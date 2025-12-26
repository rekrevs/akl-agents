# Branch Organization and Cleanup Recommendations

## Current State

**Successfully merged to main:**
- v0.9.1 tag created (828d476)
- Full x86-64 and ARM64 port complete
- Comprehensive documentation in place

**Existing branches:**
```
Active development branches:
- claude/arm64-minimal-port-011CUoHSMTFawkiKfVsuVuPP (successful ARM64 work)
- claude/x86-64-improvements-011CUoHSMTFawkiKfVsuVuPP (experimental improvements)
- claude/port-attempt-01-011CUoHSMTFawkiKfVsuVuPP (original x86-64 port)

Study/planning branches:
- claude/porting-study-011CUoHSMTFawkiKfVsuVuPP (analysis phase)
- claude/porting-study-planning-011CUoHSMTFawkiKfVsuVuPP (initial planning)

Baseline:
- origin/fresh-start (clean original codebase)
```

---

## Recommended Branch Structure

### 1. **Main Branch** (stays as-is)
```
main (v0.9.1)
├── Stable, working port for x86-64 and ARM64
├── Minimal changes from original
└── Production-ready
```

### 2. **Improvements Branch** (for code enhancements)
```
improvements
├── Base: main (v0.9.1)
├── Purpose: Speculative code improvements, refactoring, modernization
├── Examples:
│   ├── C99 modernization
│   ├── Better error handling
│   ├── Performance optimizations
│   ├── Build system enhancements (--without-fd, --without-gmp)
│   └── Memory safety improvements
└── Status: Experimental, may not all merge back to main
```

### 3. **Documentation/Book Branch** (for writing)
```
book-dev
├── Base: main (v0.9.1)
├── Purpose: "Inside AGENTS" book development
├── Contents:
│   ├── docs/book/ (existing chapters)
│   ├── Extended technical deep-dives
│   ├── Code annotations
│   ├── Architecture diagrams
│   └── Historical context
└── Status: Active development, periodic merges to main
```

---

## Recommended Actions

### Create New Branches

```bash
# 1. Create improvements branch
git checkout main
git checkout -b improvements
git push -u origin improvements

# 2. Create book development branch
git checkout main
git checkout -b book-dev
git push -u origin book-dev
```

### Archive/Delete Old Development Branches

**Keep for historical reference (archive only, no active work):**
```bash
# Tag these for history before deleting
git tag archive/attempt-01 claude/port-attempt-01-011CUoHSMTFawkiKfVsuVuPP
git tag archive/attempt-02 claude/arm64-minimal-port-011CUoHSMTFawkiKfVsuVuPP
git tag archive/improvements-experimental claude/x86-64-improvements-011CUoHSMTFawkiKfVsuVuPP
git tag archive/study claude/porting-study-011CUoHSMTFawkiKfVsuVuPP
git tag archive/planning claude/porting-study-planning-011CUoHSMTFawkiKfVsuVuPP

# Push tags
git push origin --tags

# Delete local branches
git branch -d claude/port-attempt-01-011CUoHSMTFawkiKfVsuVuPP
git branch -d claude/arm64-minimal-port-011CUoHSMTFawkiKfVsuVuPP
git branch -d claude/x86-64-improvements-011CUoHSMTFawkiKfVsuVuPP
git branch -d claude/porting-study-011CUoHSMTFawkiKfVsuVuPP
git branch -d claude/porting-study-planning-011CUoHSMTFawkiKfVsuVuPP

# Delete remote branches
git push origin --delete claude/port-attempt-01-011CUoHSMTFawkiKfVsuVuPP
git push origin --delete claude/arm64-minimal-port-011CUoHSMTFawkiKfVsuVuPP
git push origin --delete claude/x86-64-improvements-011CUoHSMTFawkiKfVsuVuPP
git push origin --delete claude/porting-study-011CUoHSMTFawkiKfVsuVuPP
git push origin --delete claude/porting-study-planning-011CUoHSMTFawkiKfVsuVuPP
```

**Keep as baseline:**
```bash
# origin/fresh-start - keep this, it's the clean original
```

---

## Branch Workflow Going Forward

### For Code Improvements
```bash
git checkout improvements
# Make changes
git add .
git commit -m "IMPROVE: Description"
git push

# When stable and tested:
git checkout main
git merge improvements
git push
```

### For Book/Documentation
```bash
git checkout book-dev
# Write documentation
git add docs/
git commit -m "BOOK: Chapter on XYZ"
git push

# Periodically merge to main:
git checkout main
git merge book-dev docs/
git push
```

### For Hotfixes on Main
```bash
git checkout main
# Fix critical bug
git add .
git commit -m "FIX: Critical bug in XYZ"
git tag v0.9.2
git push
git push --tags

# Merge back to other branches
git checkout improvements
git merge main
git push

git checkout book-dev
git merge main
git push
```

---

## What Goes Where?

### `main` branch
- ✅ Stable, tested code changes
- ✅ Critical bug fixes
- ✅ Verified documentation updates
- ✅ Release tags (v0.9.1, v0.9.2, etc.)
- ❌ Experimental changes
- ❌ Work-in-progress documentation

### `improvements` branch
- ✅ Refactoring experiments
- ✅ Performance optimizations
- ✅ Code modernization (C89 → C99+)
- ✅ Build system enhancements
- ✅ New features
- ❌ Breaking changes without migration path
- ❌ Documentation (put in book-dev)

### `book-dev` branch
- ✅ Book chapters and sections
- ✅ Extended technical documentation
- ✅ Code examples and annotations
- ✅ Architecture diagrams
- ✅ Historical analysis
- ❌ Code changes (put in improvements or main)

---

## Migration of Existing Work

### From `claude/x86-64-improvements-011CUoHSMTFawkiKfVsuVuPP`
**What to salvage:**
- `--without-fd` and `--without-gmp` configure options
- macOS compatibility fixes (if not already in main)
- Build system improvements
- FD stub functions

**Where to put it:**
```bash
git checkout improvements
git cherry-pick <commits from x86-64-improvements>
```

### From `docs/attempts/`
**What to do:**
- Keep ATTEMPT-01 and ATTEMPT-02 directories as-is (historical record)
- Consider incorporating insights into book chapters

**Where to put it:**
```bash
git checkout book-dev
# Refactor docs/attempts/ content into book chapters
git commit -m "BOOK: Incorporate porting attempt insights"
```

---

## Final Branch Structure

```
main (v0.9.1) ────────────────────────> (stable releases)
  │
  ├── improvements ───────────────────> (experimental features)
  │
  └── book-dev ───────────────────────> (documentation work)

Archives (tags, read-only):
  ├── archive/attempt-01
  ├── archive/attempt-02
  ├── archive/improvements-experimental
  ├── archive/study
  └── archive/planning

Baseline:
  └── origin/fresh-start (v0.9 original)
```

---

## Benefits of This Structure

1. **Clear separation of concerns**
   - Stable code in `main`
   - Experiments in `improvements`
   - Writing in `book-dev`

2. **Easy collaboration**
   - Different branches for different types of work
   - No conflicts between code and docs

3. **Historical preservation**
   - Archive tags preserve development history
   - Can always reference original attempts

4. **Clean history**
   - Delete working branches after archiving
   - Keeps branch list manageable

5. **Flexible merging**
   - Cherry-pick good ideas from improvements
   - Merge book chapters when ready
   - Main stays stable

---

## Summary Commands

```bash
# Create new structure
git checkout main
git checkout -b improvements && git push -u origin improvements
git checkout main
git checkout -b book-dev && git push -u origin book-dev

# Archive old branches
git tag archive/attempt-01 claude/port-attempt-01-011CUoHSMTFawkiKfVsuVuPP
git tag archive/attempt-02 claude/arm64-minimal-port-011CUoHSMTFawkiKfVsuVuPP
git tag archive/improvements-experimental claude/x86-64-improvements-011CUoHSMTFawkiKfVsuVuPP
git tag archive/study claude/porting-study-011CUoHSMTFawkiKfVsuVuPP
git tag archive/planning claude/porting-study-planning-011CUoHSMTFawkiKfVsuVuPP
git push origin --tags

# Clean up (after verifying tags are pushed)
git branch -D claude/{port-attempt-01,arm64-minimal-port,x86-64-improvements,porting-study,porting-study-planning}-011CUoHSMTFawkiKfVsuVuPP
git push origin --delete claude/{port-attempt-01,arm64-minimal-port,x86-64-improvements,porting-study,porting-study-planning}-011CUoHSMTFawkiKfVsuVuPP
```

Done! Clean, organized branch structure ready for future development.
