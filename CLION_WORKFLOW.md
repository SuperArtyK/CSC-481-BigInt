# CLion Workflow

A practical guide to Git, pull requests, and automated testing in CLion. For team rules and contribution requirements, see [Contributing](./CONTRIBUTING.md).

**Normal workflow:** Create branch → Develop → Build/test → Commit → Push → Pull request → Review → Merge.

**Remember:** Commit saves changes locally. Push uploads commits to GitHub. Neither automatically creates a pull request.

## 1. Create a feature branch

1. Use the branch widget in CLion to switch to `main`.
2. Update it using **Git → Update Project** (`Ctrl+T`).
3. Open the branch widget and select **New Branch from 'main'**.
4. Enter a descriptive name, such as `feature/addition` or `fix/division`.
5. Enable **Checkout branch** and create the branch.

Confirm that the new branch is selected before making changes.

**Important:** If you already have uncommitted changes, commit or safely preserve them before switching branches. You can also create and check out a new branch from your current branch to carry those changes over.

## 2. Build and run tests

Before committing or requesting review:

1. Select a CMake profile compatible with your compiler, such as `gcc-native-debug`.
2. Build the project using **Build → Build Project** (`Ctrl+F9`).
3. In the Run/Debug configuration selector, choose **All CTest** and click **Run** (`Shift+F10`).
4. Inspect the results in the test runner. All tests should pass.

If `All CTest` isn't available, reload CMake or look under **Run → Edit Configurations → CTest Application**. Ensure the configuration builds the necessary test targets (usually **All targets**) before running.

### Running individual tests

- Click the gutter icon beside a GoogleTest `TEST(...)` function to run or debug that test.
- Use **Rerun Failed Tests** in the test runner to repeat failures.
- When adding tests, place new `.cpp` files under the appropriate module's `tests/` directory. CMake discovers them automatically.
- GoogleTest is downloaded by CMake; no separate installation is needed.

The playground executables (`BigInt`, `BigInt_Math`, `EvaluatorDemo`) can be run separately using their own configurations. CTest does not execute these playgrounds.

## 3. Commit and push changes

1. Open **Commit** (`Alt+0`).
2. Review the diff and select the intended files.
3. Enter a descriptive commit message and click **Commit**.
4. Choose **Git → Push** (`Ctrl+Shift+K`).
5. Verify that the destination is `origin/your-branch-name`, **not `origin/main`**.
6. Click **Push**.

You can make multiple commits and pushes on the same branch. Uncommitted changes are never included in a push.

## 4. Create a pull request

1. Push your feature branch.
2. Choose **Git → GitHub → Create Pull Request**, or open the repository on GitHub.
3. Set the base branch to `main` and the head branch to your feature branch.
4. Provide a title, a description of the changes, and how you tested them.
5. Request team review. Use a **draft PR** for unfinished work.

New commits pushed to the feature branch automatically update the existing PR.

GitHub Actions runs CI for pull requests targeting `main`. Check the workflow results and resolve failures before merging.

## 5. Review someone else's pull request

1. Open **Git → GitHub → View Pull Requests**.
2. Select the PR and examine its changed files and discussion.
3. Submit your review using **Approve**, **Request Changes**, or **Comment**.

To compile or test the changes without disturbing your current work, use a separate worktree:

1. Choose **Git → Fetch** to update remote branch information.
2. Open **Git (`Alt+9`) → Worktrees → New Worktree**.
3. Select the PR branch, typically `origin/feature-name`.
4. Choose a directory outside your existing project.
5. Open the new worktree in CLion and build/test it normally.

The worktree has its own working files, so your original project's uncommitted changes remain untouched. Do not push experimental changes to the author's branch without permission.

## 6. Merge and finish

When the team agrees, the required approvals are present, review discussions are resolved, and **CI / Required** passes:

1. Merge using the pull request interface in CLion or GitHub.
2. Delete the completed feature branch when no longer needed.
3. Remove temporary review worktrees after confirming that they contain no work you need to keep.
4. Switch back to `main` and update it before starting another task.

Do not merge locally into `main` and push it as a substitute for a reviewed PR.

## Safety reminders

- Check your current branch before committing or pushing.
- Avoid switching branches with uncommitted changes unless you understand how Git will handle them.
- Never force-push to `main`.
- Reviewing a PR doesn't require checking out its branch.
- Use separate worktrees when testing other contributors' changes.
- Do not commit local build output or CLion workspace settings.

## References

- [CLion: Git branches](https://www.jetbrains.com/help/clion/manage-branches.html)
- [CLion: Commit and push](https://www.jetbrains.com/help/clion/commit-and-push-changes.html)
- [CLion: GitHub pull requests](https://www.jetbrains.com/help/clion/work-with-github-pull-requests.html)
- [CLion: Git worktrees](https://www.jetbrains.com/help/clion/use-git-worktrees.html)
- [CLion: CTest support](https://www.jetbrains.com/help/clion/ctest-support.html)