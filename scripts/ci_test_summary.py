
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
    passed = failed = skipped = 0

    for case in cases:
        name = case.get("name", "(unnamed)").replace("|", r"\|")
        time = case.get("time", "?")

        if (
                case.find("failure") is not None
                or case.find("error") is not None
        ):
            status = "Failed"
            failed += 1
        elif (
                case.find("skipped") is not None
                or case.get("status") in ("notrun", "skipped")
        ):
            status = "Skipped"
            skipped += 1
        else:
            status = "Passed"
            passed += 1

        rows.append(f"| `{name}` | {status} | {time} |")

    lines.extend([
        f"**{passed} passed, {failed} failed, "
        f"{skipped} skipped**",
        "",
        "| Test | Result | Seconds |",
        "|---|---|---:|",
        *rows,
    ])

    if failed:
        lines.extend([
            "",
            "For assertion details, inspect the "
            "**Run CTest** logs or the JUnit artifact.",
        ])

with summary_path.open("a", encoding="utf-8") as fh:
    fh.write("\n".join(lines) + "\n")
