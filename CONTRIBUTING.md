# Contributing

We use **feature branches, pull requests, code reviews, and automated tests** to maintain the project.

## Development workflow

1. **Create a branch.** Start from an up-to-date `main`. Use a separate branch for each feature, bug fix, or task.
2. **Develop and commit.** Make focused changes and commit regularly. Push your branch to GitHub whenever needed.
3. **Build and test.** Verify that the project builds and all relevant CTest tests pass locally before requesting review. Add or update unit tests when changing functionality.
4. **Open a pull request.** Target `main`, describe your changes, reference relevant issues, and explain how you tested them. Use a draft PR if the changes aren't ready for review.
5. **Review and revise.** Discuss feedback in the PR. Make corrections on the same branch and push them to update the PR automatically.
6. **Merge.** Merge only after the team agrees, review requirements are met, all discussions are resolved, and CI passes. Merge through the GitHub or CLion PR interface, not by pushing directly to `main`.

After merging, delete the completed feature branch when no longer needed and update your local `main`.

## Testing requirements

- **All automated tests must pass** before merging, including the required GitHub CI checks.
- GitHub Actions builds and tests the project using GCC, Clang, and MSVC across Debug/ASan and Release configurations.
- New functionality and bug fixes should include appropriate unit tests.
- Tests belong in the relevant subproject's `tests/` directory and are automatically discovered through CMake and GoogleTest.
- Playground `main.cpp` files are for development and experimentation. They must compile successfully but are not executed by automated CI.

## Code review

- Everyone should review proposed changes or explicitly indicate that they have no objections.
- At least **one approval** is required by repository rules. This is a minimum, not a substitute for team agreement.
- Do not merge while there are unresolved objections or requested changes.
- Keep review discussions in the pull request so decisions remain visible.
- Do not modify or push to another contributor's branch without their agreement.

## Repository rules

- Do not directly push ordinary development changes to `main`.
- Do not force-push to `main` or bypass the normal review process.
- The repository owner retains an administrative bypass for exceptional maintenance or recovery, but follows the same PR and review process during normal development.
- Prefer small, focused pull requests that are straightforward to review and test.

For practical Git, GitHub, and testing instructions in CLion, see [CLion Workflow](./CLION_WORKFLOW.md).