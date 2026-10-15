
from html import escape
from pathlib import Path
import os
import xml.etree.ElementTree as ET

preset = os.environ["CTEST_PRESET"]
report_path = Path("build") / preset / "junit.xml"
summary_path = Path(os.environ["GITHUB_STEP_SUMMARY"])

lines = [f"### CTest results — `{preset}`", ""]

if not report_path.is_file():
    lines.append(
        "No JUnit report was generated. "
        "Check the configure, build, and CTest logs."
    )
else:
    root = ET.parse(report_path).getroot()
    cases = root.findall(".//testcase")

    rows = []
    failures = []
    passed = failed = skipped = 0

    for case in cases:
        name = case.get("name", "(unnamed)")
        time = case.get("time", "?")
        failure_nodes = case.findall("failure") + case.findall("error")

        if failure_nodes:
            status = "Failed"
            failed += 1

            # CTest generally stores test output in system-out.
            # Include stderr as well when available.
            parts = [
                (case.findtext("system-out") or "").strip(),
                (case.findtext("system-err") or "").strip(),
            ]
            output = "\n\n".join(part for part in parts if part)

            # Fall back to the failure element itself.
            if not output:
                output = "\n".join(
                    (node.text or node.get("message") or "").strip()
                    for node in failure_nodes
                ).strip()

            if not output:
                output = (
                    "No diagnostic output in the JUnit report. "
                    "Inspect the CTest logs."
                )

            # Keep large diagnostic dumps from overwhelming
            # the GitHub job summary.
            if len(output) > 6000:
                output = (
                        output[:6000]
                        + "\n\n[Truncated; see CTest logs or JUnit artifact.]"
                )

            failures.append((name, output))

        elif (
                case.find("skipped") is not None
                or case.get("status") in ("notrun", "skipped")
        ):
            status = "Skipped"
            skipped += 1
        else:
            status = "Passed"
            passed += 1

        display_name = name.replace("|", r"\|").replace("\n", " ")
        rows.append(f"| `{display_name}` | {status} | {time} |")

    lines.extend([
        f"**{passed} passed, {failed} failed, "
        f"{skipped} skipped**",
        "",
        "| Test | Result | Seconds |",
        "|---|---|---:|",
        *rows,
    ])

    if failures:
        lines.extend(["", "#### Failure details", ""])

        for name, output in failures:
            lines.extend([
                "<details open>",
                f"<summary>{escape(name)}</summary>",
                "",
                "<pre>",
                escape(output),
                "</pre>",
                "",
                "</details>",
                "",
            ])

with summary_path.open("a", encoding="utf-8") as fh:
    fh.write("\n".join(lines) + "\n")
