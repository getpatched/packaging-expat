The full UTF-8 C and C++ suites run against the built candidate library.

Debian's 2.5.0-1+deb12u5 source includes upstream's XML_GE allocation tracking
but omits its matching `test_nsalloc_realloc_attributes` assertion update.
Tracking routes `expat_realloc(NULL, ...)` through `expat_malloc`, so this
particular parse must succeed without calling the injected realloc wrapper.
The bundled assertion instead expects a realloc failure.

The test-only patch backports the assertion from upstream commit
[cfce28e171676fe6f70d17b97ed8a59eaeb83f15](https://github.com/libexpat/libexpat/commit/cfce28e171676fe6f70d17b97ed8a59eaeb83f15).
It asserts zero reallocations with XML_GE=1 and retains the original failing
allocation expectations with XML_GE=0. Both test executables are rebuilt;
no test is skipped and no runtime library source or package output is changed.

[Baseline comparison run](https://github.com/getpatched/packaging-expat/actions/runs/45763680330)
at recipe commit `72927da1ebb0933457c177a6ea2e2594623591df` built the exact distro
source and our candidate in the same offline container with the same native
package-build and test commands. Before the test correction, each source failed
only this assertion: 365 checks, one failure, in both C and C++.
