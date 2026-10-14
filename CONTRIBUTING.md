# Contributing

We use **feature branches and pull requests** for all changes to `main`. Everyone on the team has repository write access, but **nobody pushes directly to `main`**.

## Making changes

1. Start from an up-to-date `main` and create a separate branch for each change (for example, `feature/settings-menu`, `fix/parser-crash`, or `docs/setup`).
2. Develop on your branch. Make clear, focused commits and push your branch to GitHub whenever you want to share or back up your progress.
3. Build and test your changes before asking for review.
4. Open a pull request (PR) **from your branch into `main`**. Explain what changed and how you tested it. A draft PR is fine if the work is not ready for review.

## Reviewing pull requests

1. Share the PR with the team. Everyone should review it or explicitly indicate that they have no objections.
2. Discuss issues in the PR. Reviewers can use **CLion worktrees** to build and test the changes without interrupting their own work.
3. If changes are requested, the author updates the same branch and PR. Re-review as needed.
4. **Merge only after the team agrees**, any requested changes are addressed, and the build/tests pass. Never merge with unresolved objections.
5. Merge using the **PR interface** in CLion or GitHub, not by locally merging into `main` and pushing it. Close a PR without merging if the team decides not to accept it.
6. After merging, delete the finished branch when no longer needed and update your local `main`.

## Guidelines

- Keep branches and PRs focused on one task whenever practical.
- Never force-push to `main` or bypass PR review.
- Do not push experimental review changes to somebody else's branch without their agreement.
- Prefer discussions and decisions in the PR so the reasoning stays visible.

For CLion-specific instructions, see [CLion workflow](./CLION_WORKFLOW.md).

## Repository setup (maintainers)

Protect `main` on GitHub: require a pull request, at least one approval from another teammate, and resolution of review conversations before merging. **One approval is only the technical minimum, not a substitute for team agreement.** Do not merge while another teammate has an unresolved objection. Configure these safeguards in GitHub repository settings.
