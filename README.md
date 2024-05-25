# MetaFL dataset

MetaFL dataset is a collection of open-source C/C++ subject programs with seeded faults, test suites, and metamorphic
relations (MRs).

It was built originally for evaluating [MetaFL](https://doi.org/10.1049/sfw2.12102).
It is open sourced here with the goal of advancing research on applying metamorphic testing to fault localization.

## Content

The subject programs contained in this dataset are summarized below.

|            Program             | Description            | Origin                                                                                 | eLOC |
|:------------------------------:|------------------------|----------------------------------------------------------------------------------------|-----:|
|         [`grep`](grep)         | Pattern matching tool  | [grep 2.4.2](https://link.springer.com/article/10.1007/s10664-005-3861-2)              | 3673 |
|          [`sed`](sed)          | Stream editor          | [sed 1.18](https://link.springer.com/article/10.1007/s10664-005-3861-2)                | 2196 |
|       [`SeqMap`](SeqMap)       | Sequences mapping tool | [SeqMap 1.0.13](https://doi.org/10.1093/bioinformatics/btn429)                         | 1261 |
| [`ShortestPath`](ShortestPath) | Find shortest paths    | [k-shortest-paths-cpp-version](https://github.com/yan-qi/k-shortest-paths-cpp-version) |  230 |
|   [`PrimeCount`](PrimeCount)   | Count primes in range  | [ktprime](https://github.com/ktprime/ktprime)                                          | 1065 |
|         [`clac`](clac)         | Stack‐based calculator | [clac](https://github.com/soveran/clac)                                                |  300 |
|        [`quich`](quich)        | Calculator             | [quich](https://github.com/Usbac/quich)                                                |  632 |

They are selected for the following reasons:

- The programs are open sourced and publicly available.
- They are cited by related studies in which MRs are defined,
  or having straightforward data processing functionality from which MRs can be clearly defined.
- Their sizes are at least hundreds of the number of executable lines of codes (eLOC).

For each subject program, we have
enumerated three MRs,
randomly generated thousands of test cases,
and generated numerous mutants with single line of fault.

The exact procedures are detailed in the [paper](https://doi.org/10.1049/sfw2.12102).
The results are summarized below.

|            Program             | MRs | Test cases | Mutants |
|:------------------------------:|----:|-----------:|--------:|
|         [`grep`](grep)         |   3 |       1000 |      75 |
|          [`sed`](sed)          |   3 |       1000 |      58 |
|       [`SeqMap`](SeqMap)       |   3 |        100 |     100 |
| [`ShortestPath`](ShortestPath) |   3 |       1000 |      21 |
|   [`PrimeCount`](PrimeCount)   |   3 |       1000 |      13 |
|         [`clac`](clac)         |   3 |       1000 |      54 |
|        [`quich`](quich)        |   3 |       1000 |      97 |

In the folder of each subject program:

- `inputs` folder contains available test suites and any supplement files required during testing.
- `mutations/20210224` folder contains the patches of valid mutants and a summary file showing fault locations.
- `source` folder stores the original source codes of the subject program
- `tools` folder contains the implementations for MRs and a tool for formatting program arguments:
    - `genMR` that generates metamorphic testing groups with a given source test case.
    - `checkMR` that validates metamorphic relations on a given group.
    - `args` that transforms a line of test case from test suite into command-line arguments for the program.

(Note that [`quickDemo`](quichDemo) and [`mid`](mid) in this repository are given as example for demo purposes)

## Usage

A typical metamorphic testing routine is as follows:

1. Prepare a version of the subject program
    1. Make a clone of the `source` folder
    2. Apply any patch file from `mutations/20210224` on the clone
    3. Compile the program
2. Prepare a metamorphic testing group
    1. Select one test case as the source test case and one MR
    2. Test the program with the source test case and collects its output
    3. Generate follow-up test cases by `genMR` with the source test case and the MR
    4. Test the program with follow-up test cases and collects their outputs
3. Validate the metamorphic relation by `checkMR` with the metamorphic testing group and the MR

## Additional resources

### Publication

Fu, L., Lei, Y., Yan, M., Xu, L., Xu, Z., & Zhang, X. (2023).
MetaFL: Metamorphic fault localisation using weakly supervised deep learning.
IET Software, 17(2), 137-153. DOI:[10.1049/sfw2.12102](https://doi.org/10.1049/sfw2.12102).

### License

MIT license. See [`LICENSE`](LICENSE) for more information.
