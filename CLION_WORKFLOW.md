# Git and pull requests in CLion 2026.2+

This is the practical companion to [the contribution guidelines](../CONTRIBUTING.md). It assumes that everyone has write access to the same GitHub repository, uses CLion 2026.2 or newer, and pushes feature branches to that repository.

**Important:** `Commit` saves a snapshot locally. `Push` publishes commits to GitHub. Neither action creates a pull request automatically.

**Normal workflow:** Create branch → Make changes → Commit → Push feature branch → Open PR → Review/test → Merge PR.

## 1. Start a new task on a branch

**Normal case — start from a clean, up-to-date `main`:**

1. Check the current branch in CLion's **VCS/branch widget** (window header).
2. If you have unfinished work on another branch, keep it there; commit it, shelve it, or use a separate worktree rather than switching away carelessly.
3. With a clean working tree, switch to local `main` using the branch widget.
4. Update `main` with **Git > Update Project** (`Ctrl+T`).
5. In the branch widget, select `main` and choose **New Branch from 'main'** (or **New Branch** while on `main`).
6. Enter a name such as `feature/settings-menu`, select **Checkout branch**, and confirm the widget shows the new branch.

**Already made uncommitted changes on `main`?** You can move those changes onto a new feature branch without committing them to `main`:

1. **While still on `main`**, open the branch widget and choose **New Branch**.
2. Name it, leave **Checkout branch** enabled, and create it. Your uncommitted changes remain in the working directory, now on the new branch.
3. Commit those changes **on the new branch**. Do not update `main`, switch to another existing branch, or discard edits first.

The new branch starts at your current local `main` commit; if `main` was behind GitHub, coordinate bringing it up to date separately once your work is safely recorded. This exception is for creating a branch **from your current branch**, not for checking out an unrelated existing branch.

**Do not commit project changes directly to `main`.**

## 2. Commit and push your work

1. Make your changes and build/test them as usual.
2. Open **Commit** (`Alt+0`). Inspect the diff and select only the files you intend to include (including new/unversioned files, if needed).
3. Enter a descriptive message and click **Commit**. You can create multiple commits while working.
4. Choose **Git > Push** (`Ctrl+Shift+K`). In the **Push Commits** dialog:
   - Check that the **local source** is your feature branch.
   - Check that the **destination** is `origin/your-branch-name`, **not `origin/main`**. Click the destination branch name to correct it if necessary.
   - Inspect the commits that will be sent.
5. Click **Push** (not **Force Push**) and check that it succeeds.
6. Continue committing and pushing to the **same branch** as you develop. You do **not** need a new branch or PR for every commit.

**First push versus later pushes:** On the first push, your new local branch is published as a new branch on GitHub (usually `origin/your-branch-name`). Later pushes add new commits to that same remote branch.

**Remember:** `Commit` records changes **locally**; `Push` sends **committed changes** to GitHub. Uncommitted edits are not included, pushing your feature branch does **not** alter `main`, and pushing does **not** create a PR. Once you open a PR, future pushes to its feature branch update that PR automatically.

If Git identity or GitHub authentication has not been configured, CLion may prompt you to set it up.

## 3. Open a pull request

1. Push your branch first.
2. Choose **Git > GitHub > Create Pull Request** (or use **Create Pull Request** in the Pull Requests window).
3. Set **base** to the shared repository's `main` branch, and **head** to your feature branch.
4. Inspect the proposed diff, enter a title and a short description (including how you tested the change), and request the team's reviews.
5. Click **Create Pull Request**. If the work is incomplete, choose **Create Draft Pull Request** instead.
6. Keep subsequent fixes on the **same feature branch** and push them; the PR updates automatically.

## 4. Inspect and discuss someone else's PR

1. Choose **Git > GitHub > View Pull Requests**.
2. Open the PR to see its commits, changed files, diff, and discussion. **Viewing these does not switch branches or change your working files.**
3. Comment on particular lines, or start a review and submit it using **Approve**, **Request changes**, or **Comment**.
4. If you also need to compile, debug, or test the PR, use a separate worktree as described below.

**Do not click `Checkout` in the PR overview of your main working project if you have uncommitted changes you want to keep in place.** That command checks out the PR branch in that working directory. Use the Worktrees workflow instead.

## 5. Create a worktree to test a PR

1. Find the PR's **head branch name** in the PR overview (for example, `feature/settings-menu`). Because contributors push to the shared repository, it should be available as a remote branch.
2. Choose **Git > Fetch** to refresh remote branch information without modifying your current working files.
3. Open **Git** (`Alt+9`), then the **Worktrees** tab. If that tab is hidden, use the tab selector in the Git tool window. Click **New Worktree**. Alternatively, choose **Git > New Worktree**.
4. In **From branch**, choose the PR's remote branch, such as `origin/feature/settings-menu`.
5. Specify a **Project name** (for example, `project-pr-42`) and a **Location outside the existing project directory**, such as `~/Projects/project-pr-42`. Do not put one worktree inside another.
6. Create the worktree. CLion opens it as another project. Build, run, debug, and test there; your original project, branch, and uncommitted edits stay where they were.
7. Leave PR feedback through the **GitHub Pull Requests** window. The worktree is your test environment; do not push experimental edits to the contributor's branch unless you have agreed to do so.

**If the PR author pushes new commits:**

1. Open the **review worktree** in CLion and ensure it has no uncommitted test edits you need to keep.
2. With the PR branch checked out and tracking `origin/feature/settings-menu`, choose **Git > Update Project** (`Ctrl+T`) to bring in the author's new commits. Do **not** select **Reset to the Remote Branch**, which can discard local commits.
3. Build/test again and refresh the PR view (`Ctrl+F5`) to read the latest comments and changes.

**Important:** A Git branch cannot be checked out in two worktrees simultaneously. If that branch is already in use locally, choose CLion's **New Branch** option when creating the worktree and give the review branch a unique name, such as `review/pr-42`. If that separate branch does not track the PR's remote branch, **Update Project** may not retrieve new PR commits; use **Git > Pull** and select the PR's remote branch, or recreate the review worktree from it.

CLion documents automatic in-editor PR Review mode when checking out a branch through the PR overview. Do **not** assume that creating a worktree independently enables this mode; you can always review the PR's diffs and comment through the Pull Requests window.

## 6. Finish the review and merge or close the PR

1. In **Git > GitHub > View Pull Requests**, open the PR.
2. Submit your review using **Approve**, **Request changes**, or **Comment**. If changes are requested, wait for the author to update the PR and review again.
3. When the team agrees, there are no unresolved objections, and tests pass, use **Merge** in the PR overview (or GitHub's PR page). Choose the team's preferred merge method.
4. If the proposal is not accepted, **Close Pull Request** instead of merging it.
5. When finished with the test environment, switch back to your regular CLion project. In **Git (`Alt+9`) > Worktrees**, select the review worktree and **Delete** it. Check first that it has no uncommitted work you need. CLion cannot delete the worktree currently open in that project window.
6. Delete the merged feature branch when it is no longer needed. Update your local `main` the next time you work from it.

## Safety notes

- **Safe with uncommitted changes:** viewing PRs/diffs, commenting, approving, **Git > Fetch**, and creating a *separate* worktree.
- **Use care:** checking out another branch, pulling/merging/rebasing, deleting worktrees, and discarding edits.
- A worktree has its own files and uncommitted changes, but worktrees share the repository's Git history and branch references.
- Do not commit `.idea/workspace.xml`; JetBrains warns that a shared `ProjectId` can make separate worktrees appear to be the same CLion project. Ignore local build output as well.

## Official reference documentation

- [CLion: Manage Git branches](https://www.jetbrains.com/help/clion/manage-branches.html)
- [CLion: Commit and push changes](https://www.jetbrains.com/help/clion/commit-and-push-changes.html)
- [CLion: Create GitHub pull requests](https://www.jetbrains.com/help/clion/create-and-merge-github-pull-requests.html)
- [CLion: Review GitHub pull requests](https://www.jetbrains.com/help/clion/work-with-github-pull-requests.html)
- [CLion: Use Git worktrees](https://www.jetbrains.com/help/clion/use-git-worktrees.html)
- [CLion: Fetch, pull, and update](https://www.jetbrains.com/help/clion/sync-with-a-remote-repository.html)

## One-time GitHub repository setup (maintainers)

To enforce the agreement even though everyone has write access, configure a GitHub branch protection rule or ruleset for `main`: **require a pull request before merging**, **require at least one approval**, and **require conversation resolution**. This is done in GitHub repository settings, not inside CLion. See [GitHub: Managing a branch protection rule](https://docs.github.com/en/repositories/configuring-branches-and-merges-in-your-repository/managing-protected-branches/managing-a-branch-protection-rule).
