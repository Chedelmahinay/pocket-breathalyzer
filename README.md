# iot-project (edit #1)

### Conventional Commits

In this project, **Conventional Commits** are used, which is a simple set of rules to structure and create an explicit commit history that is easy to read both for humans and machines if needed.

Learn more about describing the features, fixes, and breaking changes made in commit messages here: [Click Me](https://www.conventionalcommits.org/en/v1.0.0/#specification)

#### The commit message should be structured as follows:
```<type>(scope): <description>```

#### The commit contains the following structural elements:

- **fix**: a commit of the ```type``` ___fix___ patches a bug in your codebase.

- **feat**: a commit of the ```type``` ___feat___ introduces a new feature to the codebase.

- **!**: a commit that appends a ___!___ after the ```scope```, introduces a ___breaking change___. A breaking change can be part of commits of any type.

#### Examples:

Commit message with ```!``` to draw attention to a ___breaking change___:

- ```feat(api)!: send an email to the customer when a product is shipped```

Commit message with ```scope``` lang:

- ```feat(lang): add Polish language```

Commit message with ```type``` fix:

- ```fix(ui): css syntax error```

### Branching Strategy

A **branching strategy** is a set of rules or guidelines that development teams use to manage the process of writing, merging, and deploying code with the help of a version control system like **Git**. It defines how developers interact with a shared codebase.

Strategies like these are essential as they help in keeping project repositories organized, error-free, and **avoid the unwanted merge conflicts** when multiple developers simultaneously push and pull code from the same repository.

To learn more about branching strategies: [Click Me](https://www.geeksforgeeks.org/git/branching-strategies-in-git/)

A _modified_ version of the **GitLab Flow** is used in this project.

#### GitLab Flow (modified):

The types of branches that can be present in GitFlow are:

- **master/main**: Main production branch housing stable release ready code.

- **dev**: Contains new features and bug fixes. This branch is used to combine new features and bug fixes for the release. Upon completion, developers merge the changes from the ```dev``` branch into the ```main/master``` branches.

- **feature/<scope<d>>**: Developers initiate ```feature/*``` branch from the ```dev``` branch to implement new features or address bugs. Upon completion, they integrate the changes from the ```feature/*``` branch into the ```dev``` branch. **For example:**
    - ```feature/ui```: To work on the UI of the project
    - ```feature/api```: To work on the API calls of the project