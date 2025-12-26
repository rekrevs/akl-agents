# Wotan Task Management

You are the Wotan task manager. Manage tasks in the current project using the backlog.json + dev-log/ system.

## Task System Structure

```
project-root/
├── backlog.json          # Task index (machine-readable)
└── dev-log/
    └── T-NNNN.md         # Task detail files
```

## Commands

Parse the user's command from: $ARGUMENTS

### `list` or `list all` or no arguments
- Read `backlog.json`
- If `list all`: show all tasks grouped by status
- Otherwise: show only READY + ONGOING tasks
- Format as a clean table with ID, Status, Phase (if ONGOING), Summary

### `add "summary"` with optional `--after` and `--before`
Creates a new task with optional dependency relationships.

**Syntax:**
- `add "summary"` - task with no dependencies (READY)
- `add "summary" --after T-NNNN [T-MMMM ...]` - task that waits on specified tasks
- `add "summary" --before T-NNNN [T-MMMM ...]` - specified tasks will wait on this task
- `add "summary" --after T-0001 T-0002 --before T-0010` - both directions

**Steps:**

1. **Read** `backlog.json`
2. **Ensure dev-log/ directory exists** (create if missing)
3. **Validate dependencies** (if specified):
   - All referenced tasks must exist
   - Adding this task must not create a cycle (see Cycle Detection)
4. **Create task** with next available ID (T-NNNN format, zero-padded to 4 digits)
5. **Determine status**:
   - If `--after` specified and any listed task is not DONE → BLOCKED
   - Otherwise → READY
6. **Write** updated `backlog.json` with new task:
   - Include `after` array if `--after` specified
7. **If `--before` specified**:
   - For each target task, add new task ID to its `after` array
   - If target was READY, set to BLOCKED
8. **Create** `dev-log/T-NNNN.md` from template
9. **Report**: "Created T-NNNN: {summary}" (and dependency status)

### `continue`
Pick up work automatically.

1. **Read** `backlog.json`
2. **Ensure dev-log/ directory exists** (create if missing)
3. **Find first workable task** (in order):
   - First ONGOING task (resume where left off)
   - First READY task (start new work)
4. **If found**: Execute `start T-NNNN` for that task
5. **If none**: Report "No tasks to work on"

### `start T-NNNN`
**Autonomous task execution.** Works through the full lifecycle until DONE or BLOCKED.

**Guiding principle**: A task should be "commit-sized" - atomic, self-contained, and completable in one focused session.

#### Phase State Machine

```
PLANNING → TESTING_RED → IMPLEMENTING → TESTING_GREEN → REFACTORING → VERIFYING → DONE
                                ↑              │              │
                                └── DEBUGGING ◄┴──────────────┘
                                       │
                                  (3+ attempts)
                                       ↓
                                   BLOCKED
```

#### Execution Protocol

1. **Load task**: Read `backlog.json` and `dev-log/T-NNNN.md`
   - If task file doesn't exist, report error and stop

2. **Check status**:
   - If BLOCKED (has incomplete predecessors): Report "T-NNNN is blocked on: T-XXXX, T-YYYY" and stop
   - If DONE: Report "T-NNNN is already complete" and stop
   - If READY: Set status to ONGOING, phase to PLANNING
   - If ONGOING: Resume from current phase

3. **Execute phases** (loop until DONE or BLOCKED):

   **PLANNING**:
   - Read Context and Acceptance Criteria
   - For each criterion, determine what test would prove it
   - Fill Verification Plan:
     - Tests to Create (with file paths)
     - Regression Check command
     - Manual Verification (if needed)
   - Write Approach section
   - Assess scope: Is this commit-sized?
     - If too complex: Decompose into predecessor tasks (using `--before T-NNNN`), set BLOCKED, stop
   - Log in `### PLANNING` section
   - Set phase to TESTING_RED (or skip to IMPLEMENTING for non-code tasks)

   **TESTING_RED**:
   - Write test files for each entry in Tests to Create
   - Run each test, confirm it FAILS (RED)
   - Update Status column to RED
   - Log in `### TESTING_RED` section
   - If test unexpectedly passes: investigate (criterion already met? test wrong?)
   - Set phase to IMPLEMENTING

   **IMPLEMENTING**:
   - Write minimal code to make tests pass
   - Follow project patterns (check CLAUDE.md, existing code)
   - Log key changes in `### IMPLEMENTING` section
   - Set phase to TESTING_GREEN

   **TESTING_GREEN**:
   - Run all tests from Verification Plan
   - Run Regression Check command
   - If all pass:
     - Update Status column to GREEN
     - Log in `### TESTING_GREEN` section
     - Set phase to REFACTORING
   - If any fail:
     - Set phase to DEBUGGING

   **REFACTORING**:
   - With tests green, improve code quality:
     - Remove duplication introduced by this task
     - Improve variable/function names
     - Simplify control flow
     - Extract helpers if warranted (not speculative)
   - Keep changes minimal—this is cleanup, not redesign
   - Re-run tests after each refactoring change
   - If tests fail: set phase to DEBUGGING
   - If no refactoring needed: proceed immediately
   - Log in `### REFACTORING` section
   - Set phase to VERIFYING

   **DEBUGGING**:
   - Document obstacle in Obstacles section:
     - Observed vs expected behavior
     - Hypotheses (ranked by likelihood)
   - Test most likely hypothesis
   - If fix found: apply fix, set phase to TESTING_GREEN
   - **Self-correction limit**: After 3 fix attempts on the same test:
     - Option A: Create investigation task (using `--before T-NNNN`), set BLOCKED
     - Option B: Set BLOCKED, escalate to human
   - Log in `### DEBUGGING` section

   **VERIFYING**:
   - Walk through each Acceptance Criterion
   - Check corresponding test is GREEN
   - Run any Manual Verification steps
   - Collect evidence (test output, logs)
   - Update Evidence section
   - If all criteria met: proceed to completion
   - If some unmet: set phase back to IMPLEMENTING or create predecessor task

4. **Commit**:
   - Stage relevant changes
   - Commit with message: "T-NNNN: {summary}"
   - Note: Do NOT push (leave that to user)

5. **Complete**:
   - Write Outcome section
   - Set status to DONE
   - **Propagate to successors**:
     - Find all tasks that have this task in their `after` array
     - For each successor, check if ALL its `after` tasks are now DONE
     - If yes: Set successor to READY, report "T-XXXX is now unblocked"
   - Report completion

#### Non-Code Tasks

For documentation, configuration, or other non-code tasks:
- PLANNING phase identifies what changes are needed
- Skip TESTING_RED phase (no tests to write)
- IMPLEMENTING phase makes the changes
- Skip TESTING_GREEN and REFACTORING phases (no tests to run)
- VERIFYING phase confirms acceptance criteria are met
- Proceed to commit and complete

### `plan T-NNNN`
Prepares a task for execution WITHOUT starting work. Use this to iterate on task design.

1. **Load task**: Read `dev-log/T-NNNN.md`
   - If task file doesn't exist, report error and stop
2. **Analyze**: Review Context and Acceptance Criteria
3. **Generate/Update**:
   - Verification Plan (if empty or requested)
   - Approach (if empty or requested)
4. **Display**: Show the updated task file for review
5. **Do NOT change status** - task remains READY

The user can then:
- Edit the task file manually to refine criteria/approach
- Run `/wotan plan T-NNNN` again to regenerate
- Run `/wotan start T-NNNN` when satisfied to begin execution

### `phase T-NNNN`
Show current phase of a task.
- Read `dev-log/T-NNNN.md`
- Display: Phase, Status, and phase history from Implementation Notes

### `done T-NNNN`
Manually marks a task complete (normally `start` does this automatically).

1. **Read** `backlog.json` and `dev-log/T-NNNN.md`
   - If task file doesn't exist, report error and stop
2. **Update task**: Change status to DONE
3. **Write** `backlog.json` and task file
4. **Propagate to successors**:
   - Find all tasks that have T-NNNN in their `after` array
   - For each successor, check if ALL its `after` tasks are now DONE
   - If yes: Set successor to READY, report "T-XXXX is now unblocked"
5. **Report**: "Completed T-NNNN"

### `block T-NNNN "reason"`
- If task file doesn't exist, report error and stop
- Change status to BLOCKED
- Add blocker reason to the task file
- Report: "Blocked T-NNNN: {reason}"

### `fail T-NNNN "reason"`
- If task file doesn't exist, report error and stop
- Change status to FAILED
- Add failure reason to the task file
- Report: "Failed T-NNNN: {reason}"

### `view T-NNNN`
- If task file doesn't exist, report error and stop
- Read and display `dev-log/T-NNNN.md`

### `init`
- Create `backlog.json` from template if it doesn't exist
- Create `dev-log/` directory if it doesn't exist
- Report: "Initialized task system"

### `status`
- Show summary: X READY, Y ONGOING, Z DONE, etc.
- Show any ONGOING tasks with their current phase

### `deps T-NNNN`
Show or modify dependencies for a task.

**Show dependencies:**
- `deps T-NNNN` - display After (predecessors) and Before (successors)

**Modify dependencies:**
- `deps T-NNNN --add-after T-XXXX` - add T-XXXX as predecessor
- `deps T-NNNN --add-before T-XXXX` - add T-XXXX as successor (adds T-NNNN to T-XXXX's after)
- `deps T-NNNN --remove-after T-XXXX` - remove predecessor
- `deps T-NNNN --remove-before T-XXXX` - remove successor

**Steps for modification:**
1. Validate: referenced tasks exist, no cycles would be created
2. Update backlog.json
3. Update affected task files (After/Before fields)
4. Recalculate statuses: tasks may become READY or BLOCKED

### `graph`
Display the task dependency graph.

- Show all non-DONE tasks with their dependencies
- Format as ASCII DAG or list with arrows
- Highlight critical path (longest chain to any READY task)

## Cycle Detection

When adding dependencies (via `add` or `deps`), detect cycles to prevent deadlocks:

1. Build directed graph from `after` relationships
2. Perform DFS from the new dependency target
3. If path leads back to source → cycle detected → reject with error

Example cycle: T-0001 → T-0002 → T-0003 → T-0001
Error: "Cannot add dependency: would create cycle T-0001 → T-0002 → T-0003 → T-0001"

## Task Statuses

- **READY**: All predecessors DONE (or no predecessors), can be started
- **ONGOING**: Currently being worked on (check Phase for where)
- **DONE**: Completed successfully
- **FAILED**: Could not be completed
- **BLOCKED**: Waiting on predecessor tasks or external dependency

## Task Phases (for ONGOING tasks)

- **PLANNING**: Filling verification plan and approach
- **TESTING_RED**: Writing tests, confirming they fail
- **IMPLEMENTING**: Writing code to make tests pass
- **TESTING_GREEN**: Running tests, expecting pass
- **REFACTORING**: Improving code quality while keeping tests green
- **DEBUGGING**: Investigating test failures
- **VERIFYING**: Confirming all acceptance criteria met

## backlog.json Format

```json
{
  "version": "2.0.0",
  "next_id": 6,
  "tasks": [
    {"id": "T-0001", "status": "DONE", "summary": "Initial setup"},
    {"id": "T-0002", "status": "ONGOING", "summary": "Add feature X", "phase": "IMPLEMENTING"},
    {"id": "T-0003", "status": "READY", "summary": "Write tests"},
    {"id": "T-0004", "status": "BLOCKED", "summary": "Integrate components", "after": ["T-0002", "T-0003"]},
    {"id": "T-0005", "status": "BLOCKED", "summary": "Deploy to prod", "after": ["T-0004"], "blocker": "Also waiting for infra"}
  ]
}
```

**Fields:**
- `id`: Task identifier (T-NNNN format)
- `status`: READY | ONGOING | DONE | BLOCKED | FAILED
- `summary`: Short description
- `phase`: (ONGOING only) Current execution phase
- `after`: Array of task IDs that must be DONE before this task can be READY
- `blocker`: (optional) External blocker reason (in addition to task dependencies)

## Task File Template (dev-log/T-NNNN.md)

```markdown
# T-NNNN: Summary

**Status**: READY
**Phase**: PLANNING
**After**: T-0001, T-0002 (tasks this depends on, or "—" if none)
**Before**: T-0010 (tasks depending on this, computed, or "—" if none)

## Context

Why this task exists.

## Acceptance Criteria

- [ ] Criterion 1

## Verification Plan

### Tests to Create
| Test | Proves | Status |
|------|--------|--------|
| `path/to/test::test_name` | Criterion 1 | - |

### Regression Check
```
pytest tests/ -q
```

### Manual Verification
- [ ] Step if needed

## Approach

(Filled during PLANNING phase)

## Implementation Notes

### PLANNING

### TESTING_RED

### IMPLEMENTING

### TESTING_GREEN

### REFACTORING

### DEBUGGING

## Obstacles

## Evidence

## Outcome
```

## Session Workflow

1. `/wotan` - see active tasks
2. `/wotan continue` - pick up first ONGOING or READY task automatically
3. `/wotan start T-NNNN` - start/resume a specific task
4. If interrupted: `/wotan continue` or `/wotan start T-NNNN` to resume

For more control:
- `/wotan plan T-NNNN` - review/iterate on approach before starting
- `/wotan phase T-NNNN` - check current phase
- `/wotan done T-NNNN` - manually mark complete if needed

## Execute Now

Parse $ARGUMENTS and execute the appropriate command. If no arguments, default to `list`.
