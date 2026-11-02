# GetPatched Expat recipe

This repository is a public maintenance recipe for Debian's Expat source package. It keeps the upstream source separate from Debian's packaging, pins one exact Debian Security source per target, and carries a narrowly scoped backport with its regression probe.

## Candidate targets

| Target | Exact Debian source | Candidate version | Status |
|---|---|---|---|
| Debian 12 Bookworm amd64 | `2.5.0-1+deb12u5` | `2.5.0-1+deb12u5+getpatched1` | Development candidate; installed package checks pending |
| Debian 13 Trixie amd64 | `2.9.0-1~deb13u1` | `2.9.0-1~deb13u1+getpatched1` | Development candidate; installed package checks pending |

Both source descriptors and their orig and Debian packaging archives are pinned by SHA-256 in the target files. The hashes match the signed Debian Security `Sources.xz` indexes for immutable snapshot `20261030T000000Z`. The `.dsc` uploader signatures have not been verified separately. Ubuntu, other Debian releases, arm64, and other architectures are not configured here.

## Vulnerability and backport

CVE-2026-117610 is a NULL pointer dereference in `externalEntityInitProcessor2()`. An application that handles external entities can crash when a subparser first receives an empty non-final chunk and later receives document data. Debian's current tracker marks both pinned Security source versions vulnerable. Upstream fixed the issue in commit [`bfc28346fa3efeb8ca56c17437d24a7344e5feab`](https://github.com/libexpat/libexpat/commit/bfc28346fa3efeb8ca56c17437d24a7344e5feab) by handling `XML_TOK_NONE` during initialization. The same small patch applies to both source versions after their Debian quilt series.

The regression fixture calls the installed library through its public C API. It creates an external-entity parser, parses an empty non-final input chunk, resumes with a processing instruction, and verifies the handler ran. A separate `xmlwf -v` check covers a normal command-line consumer.

## Evidence state

The exact `.dsc`, upstream source, and Debian packaging archives for both targets were downloaded and matched against their signed index hashes. Each Debian quilt series applied with zero fuzz, and the GetPatched backport applied to both targets without fuzz. On the local arm64 macOS host, Debian's patched source built with its Autotools flow: the regression crashed with SIGSEGV before the backport and passed after it for both source versions. This is source-build evidence only; the target-specific Debian `.deb` build, installed-package regression, package-aware scans, consumer checks, and signed APT client verification are pending. This candidate is not counted as a completed fix or release.

## Licensing

The repository's `LICENSE` covers GetPatched-authored recipe metadata and documentation. Expat and its upstream fix are MIT licensed; the original upstream copyright and Debian notices are retained in `LICENSES/`. Preserve those notices in source and binary packages.
