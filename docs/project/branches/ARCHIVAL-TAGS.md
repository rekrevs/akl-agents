# Archival Tags for Branch Cleanup

This file documents the archival tags to create before deleting old development branches.

## Status Summary

| Tag | Branch | Status | Action Needed |
|-----|--------|--------|---------------|
| archive/attempt-01 | claude/port-attempt-01-011CUoHSMTFawkiKfVsuVuPP | ✅ Created | Delete branch |
| archive/arm64-minimal-port | claude/arm64-minimal-port-011CUoHSMTFawkiKfVsuVuPP | ✅ Created | Delete branch |
| archive/x86-64-improvements | claude/x86-64-improvements-011CUoHSMTFawkiKfVsuVuPP | ⏳ Pending | Create tag, delete branch |
| archive/study | claude/porting-study-011CUoHSMTFawkiKfVsuVuPP | ⏳ Pending | Create tag, delete branch |
| archive/planning | claude/porting-study-planning-011CUoHSMTFawkiKfVsuVuPP | ⏳ Pending | Create tag, delete branch |

---

## Tags to Create

**Note:** All tags must be created in your local repository using `origin/` prefix, since the branches only exist on remote.

### archive/x86-64-improvements ⏳

**Purpose:** Preserve the x86-64-improvements branch history before deletion

**Commands:**
```bash
git fetch origin
git tag archive/x86-64-improvements origin/claude/x86-64-improvements-011CUoHSMTFawkiKfVsuVuPP
git push origin archive/x86-64-improvements
```

**Why:** Contains 27 commits from failed ARM64 macOS porting attempt. Valuable documentation extracted to book-dev. See `X86-64-IMPROVEMENTS-BRANCH-ANALYSIS.md`.

**After tag is pushed:**
```bash
git push origin --delete claude/x86-64-improvements-011CUoHSMTFawkiKfVsuVuPP
```

---

### archive/study ⏳

**Purpose:** Preserve initial porting study/planning phase

**Commands:**
```bash
git fetch origin
git tag archive/study origin/claude/porting-study-011CUoHSMTFawkiKfVsuVuPP
git push origin archive/study
```

**Why:** Initial analysis phase (docs 01-11). Superseded by main and book-dev. See `PORTING-STUDY-ANALYSIS.md`.

**After tag is pushed:**
```bash
git push origin --delete claude/porting-study-011CUoHSMTFawkiKfVsuVuPP
```

---

### archive/planning ⏳

**Purpose:** Preserve temporary working branch from cleanup session

**Commands:**
```bash
git fetch origin
git tag archive/planning origin/claude/porting-study-planning-011CUoHSMTFawkiKfVsuVuPP
git push origin archive/planning
```

**Why:** Created organization docs (BRANCH-ORGANIZATION.md, etc.) - all migrated to book-dev. See `PORTING-STUDY-PLANNING-ANALYSIS.md`.

**After tag is pushed:**
```bash
git push origin --delete claude/porting-study-planning-011CUoHSMTFawkiKfVsuVuPP
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
