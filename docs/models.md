## Model

### Router

The router model is:
- input buffered router with VCs
- variable number of VCs and buffer depths for each VC
- round robin arbitration over VCs (rrVC) or fair arbitration over used VCs (fair)
- routing functions implemented in routing; different deterministic and adaptive routings are given
- selection functions implemented in selection
- supports synchronous and asynchronous clock speeds of layers

To run the uniform random simulation please go to <a href="https://github.com/jmjos/ratatoskr/tree/master/scripts/urand">README.md</a>
