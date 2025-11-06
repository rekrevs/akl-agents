# Archival Tags for Branch Cleanup

This file documents the archival tags to create before deleting old development branches.

## Tags to Create

### archive/x86-64-improvements

**Purpose:** Preserve the x86-64-improvements branch history before deletion

**Command:**
```bash
git tag archive/x86-64-improvements claude/x86-64-improvements-011CUoHSMTFawkiKfVsuVuPP
git push origin archive/x86-64-improvements
```

**Why:** This branch contains 27 commits representing a failed ARM64 macOS porting attempt. The valuable documentation has been extracted to `claude/book-dev-011CUoHSMTFawkiKfVsuVuPP`, but the complete history is preserved via this tag.

**Commit hash:** `9ffa035` (HEAD of improvements branch)

**Branch can be deleted after tag is created:**
```bash
git push origin --delete claude/x86-64-improvements-011CUoHSMTFawkiKfVsuVuPP
git branch -D claude/x86-64-improvements-011CUoHSMTFawkiKfVsuVuPP
```

---

### Other Recommended Archive Tags

Per `BRANCH-ORGANIZATION.md`, you may also want to archive:

#### archive/attempt-01
```bash
git tag archive/attempt-01 claude/port-attempt-01-011CUoHSMTFawkiKfVsuVuPP
git push origin archive/attempt-01
```

#### archive/attempt-02
```bash
git tag archive/attempt-02 claude/arm64-minimal-port-011CUoHSMTFawkiKfVsuVuPP
git push origin archive/attempt-02
```

#### archive/study
```bash
git tag archive/study claude/porting-study-011CUoHSMTFawkiKfVsuVuPP
git push origin archive/study
```

#### archive/planning
```bash
git tag archive/planning claude/porting-study-planning-011CUoHSMTFawkiKfVsuVuPP
git push origin archive/planning
```

---

## Verification

After creating tags, verify they exist:
```bash
git tag -l "archive/*"
git show archive/x86-64-improvements --stat
```

## Recovery

If you ever need to restore a branch from a tag:
```bash
git checkout -b recovered-branch archive/x86-64-improvements
```

---

**Note:** These commands should be run in your local repository, not in the Claude environment, since tags need to be pushed to origin for persistent access.
