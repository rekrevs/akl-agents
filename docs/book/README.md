# Inside AGENTS: A Concurrent Constraint Programming System

**A deep dive into porting, understanding, and modernizing a 1990s research VM**

---

## About This Book

This book documents the journey of porting the AGENTS v0.9 concurrent constraint programming system from SPARC to x86-64, along with deep technical insights into its architecture. Along the way, we discovered and fixed a 30-year-old bug, proving that thorough software archaeology can improve even well-tested codebases.

**What makes this unique:**
- Real-world porting case study with complete documentation
- Discovery of fundamental bug affecting all 64-bit platforms
- Deep technical analysis of 1990s VM design
- Lessons applicable to modern systems

**Who this is for:**
- Systems programmers interested in VM implementation
- Those maintaining or porting legacy code
- Students learning about bytecode interpreters
- Anyone curious about constraint programming systems

---

## Table of Contents

### [Part I: Introduction](01-introduction/)

**[Chapter 1: What is AGENTS?](01-introduction/what-is-agents.md)**
- Origins at Swedish Institute of Computer Science (SICS)
- Concurrent constraint programming paradigm
- Why it matters historically

**[Chapter 2: The History](01-introduction/history.md)**
- Development timeline (1990-1994)
- Research context and goals
- Legacy and influence

**[Chapter 3: About This Book](01-introduction/this-book.md)**
- How to read this book
- What you'll learn
- Source code availability

---

### [Part II: Architecture](02-architecture/)

**[Chapter 4: System Overview](02-architecture/overview.md)**
- High-level architecture
- Component interactions
- Design philosophy

**[Chapter 5: The Emulator](02-architecture/emulator.md)**
- Bytecode instruction set
- Threaded code dispatch
- VM registers and state

**[Chapter 6: Memory Management](02-architecture/memory-management.md)**
- Two-space copying garbage collector
- Tagged pointers and type system
- Heap organization

**[Chapter 7: Bytecode Dispatch](02-architecture/bytecode-dispatch.md)**
- Computed goto technique
- Instruction encoding
- Performance considerations

**[Chapter 8: The Build System](02-architecture/build-system.md)**
- 1993-era autoconf
- Compiler requirements
- Platform detection

---

### [Part III: The Porting Story](03-porting-story/)

**[Chapter 9: The Challenge](03-porting-story/the-challenge.md)**
- Why port AGENTS to x86-64?
- Initial assessment
- Risk analysis

**[Chapter 10: Planning Phase](03-porting-story/planning-phase.md)**
- Studying existing ports (Alpha, SPARC)
- Identifying platform dependencies
- Creating the execution plan

**[Chapter 11: Execution](03-porting-story/execution.md)**
- Phase-by-phase implementation
- Compilation success
- Runtime validation

**[Chapter 12: The Bug Discovery](03-porting-story/discoveries.md)** ⭐
- Boot failures with NOBIGNUM
- Investigation methodology
- The MaxSmallNum signedness bug
- Impact on all platforms

**[Chapter 13: Lessons Learned](03-porting-story/lessons-learned.md)**
- What worked
- What didn't
- Key insights

---

### [Part IV: Technical Deep Dives](04-technical-deep-dives/)

**[Chapter 14: Tagged Pointers](04-technical-deep-dives/tagged-pointers.md)**
- Encoding types in pointer bits
- TADBITS configuration
- Trade-offs and limits

**[Chapter 15: Garbage Collection](04-technical-deep-dives/garbage-collection.md)**
- Two-space copying algorithm
- Root scanning
- Performance characteristics

**[Chapter 16: Constraint Solving](04-technical-deep-dives/constraint-solving.md)**
- Finite domain constraints
- Propagation mechanisms
- Suspension and waking

**[Chapter 17: Register Allocation](04-technical-deep-dives/register-allocation.md)**
- Hard register assignment
- Platform-specific mappings
- Performance impact

**[Chapter 18: The Signedness Bug](04-technical-deep-dives/signedness-bug.md)** ⭐
- Complete technical analysis
- Why it existed for 30 years
- How we found it
- The fix and its implications

---

### [Part V: Platform Specifics](05-platform-specifics/)

**[Chapter 19: SPARC Implementation](05-platform-specifics/sparc.md)**
- Original platform
- Register usage
- Calling conventions

**[Chapter 20: Alpha Implementation](05-platform-specifics/alpha.md)**
- First 64-bit port
- Minimal configuration approach
- Lessons for x86-64

**[Chapter 21: x86-64 Implementation](05-platform-specifics/x86-64.md)**
- Complete porting guide
- Register mapping
- ABI considerations

**[Chapter 22: Porting to New Platforms](05-platform-specifics/porting-guide.md)**
- Step-by-step methodology
- Platform requirements
- Testing strategy

---

### [Part VI: Appendices](06-appendices/)

**[Appendix A: Build Instructions](06-appendices/build-system.md)**
- Prerequisites
- Platform-specific notes
- Troubleshooting

**[Appendix B: Testing](06-appendices/testing.md)**
- DejaGnu test suite
- Runtime validation
- Regression testing

**[Appendix C: References](06-appendices/references.md)**
- Papers and publications
- Related systems
- Further reading

**[Appendix D: Historical Artifacts](06-appendices/historical-artifacts.md)**
- Original documentation
- Attempt logs
- Discovery process

---

## How to Use This Book

**For porters:**
- Start with Part III (The Porting Story)
- Reference Part V (Platform Specifics)
- Use Part VI (Appendices) for practical details

**For VM enthusiasts:**
- Begin with Part II (Architecture)
- Deep dive into Part IV (Technical Deep Dives)
- Study the signedness bug case study (Chapters 12 & 18)

**For learners:**
- Read sequentially from Part I
- Work through examples
- Experiment with the code

**For researchers:**
- Part I for context
- Part IV for technical depth
- Appendix C for references

---

## Key Highlights

🎉 **Major Discovery:** Found and fixed a 30-year-old signedness bug affecting all 64-bit platforms

✅ **Complete Port:** Fully working x86-64 implementation with ~30 lines of code

📚 **Comprehensive Documentation:** Every step documented from planning to breakthrough

🔬 **Real-World Case Study:** Actual software archaeology with all the messy details

---

## Source Code

The complete source code, including:
- Original AGENTS v0.9 (SPARC/Alpha)
- x86-64 port with all improvements
- Bug fixes and modernizations
- Complete git history

Available at: https://github.com/rekrevs/akl-agents

Branch: `x86-64-improvements`

---

## Contributing

Found issues or have improvements? See the contributing guidelines in the repository.

---

## License

AGENTS v0.9: (C) 1990-1994 Swedish Institute of Computer Science

This documentation: CC BY-SA 4.0

---

## Acknowledgments

- Original AGENTS developers at SICS
- The constraint programming research community
- Everyone who believes in preserving and understanding legacy systems

---

**Last Updated:** 2025-11-05
**Version:** 1.0
**Status:** Work in Progress

---

*This book is both a technical manual and a story of software archaeology - proving that understanding the past helps us build better futures.*
