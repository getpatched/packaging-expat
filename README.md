# GetPatched Expat recipe

This repository is a public maintenance recipe for Debian's Expat source package. It keeps the upstream source separate from Debian's packaging, pins one exact Debian Security source per target, and carries a narrowly scoped backport with its regression probe.

## Candidate targets

| Target | Exact Debian source | Candidate version | Status |
|---|---|---|---|
| Debian 12 Bookworm amd64 | `2.5.0-1+deb12u5` | `2.5.0-1+deb12u5+getpatched1` | CVE-2026-117610 candidate; hosted checks passed |
| Debian 13 Trixie amd64 | `2.9.0-1~deb13u1` | `2.9.0-1~deb13u1+getpatched2` | CVE-2026-117610 verified candidate plus CVE-2026-113351 verification pending |

Both source descriptors and their orig and Debian packaging archives are pinned by SHA-256 in the target files. The hashes match the signed Debian Security `Sources.xz` indexes for immutable snapshot `20261030T000000Z`. The `.dsc` uploader signatures have not been verified separately. Ubuntu, other Debian releases, arm64, and other architectures are not configured here.

## Vulnerability and backport

CVE-2026-117610 is a NULL pointer dereference in `externalEntityInitProcessor2()`. An application that handles external entities can crash when a subparser first receives an empty non-final chunk and later receives document data. Debian's current tracker marks both pinned Security source versions vulnerable. Upstream fixed the issue in commit [`bfc28346fa3efeb8ca56c17437d24a7344e5feab`](https://github.com/libexpat/libexpat/commit/bfc28346fa3efeb8ca56c17437d24a7344e5feab) by handling `XML_TOK_NONE` during initialization. The same small patch applies to both source versions after their Debian quilt series.

The regression fixture calls the installed library through its public C API. It creates an external-entity parser, parses an empty non-final input chunk, resumes with a processing instruction, and verifies the handler ran. A separate `xmlwf -v` check covers a normal command-line consumer.

The Trixie candidate also backports CVE-2026-113351, an allocator-accounting denial of service after `XML_ParserReset()`. The new public-API probe parses a valid 70 MiB attribute value, resets the same parser, and parses a small document. It expects the exact vulnerable Trixie binary to fail that second parse with `XML_ERROR_NO_MEMORY` and the patched binary to accept it. Debian Bookworm's Expat 2.5.0 predates the affected allocation tracker, so this fix is intentionally Trixie-only. The backport is upstream Expat PR [#1431](https://github.com/libexpat/libexpat/pull/1431), applied without fuzz to the pinned Expat 2.9.0 source after Debian's quilt series.

## Evidence state

The exact `.dsc`, upstream source, and Debian packaging archives for both targets are pinned by the hashes in the target files. Earlier hosted Bookworm and Trixie verification of the `+getpatched1` CVE-2026-117610 candidate built installed Debian packages, reproduced the baseline SIGSEGV, passed the patched regression and `xmlwf` check, cleared the target from the candidate package-aware scan, and installed through isolated signed APT clients. Trivy 0.70.0 does not know CVE-2026-117610; its no-finding result is not used as proof. The CVE-2026-113351 patch applies without fuzz after Trixie's exact Debian quilt series, but the new `+getpatched2` candidate still needs its installed-package regressions, paired scans, consumer checks, and signed APT client test. No binaries or signed production indexes are published, and the new Trixie candidate is not yet counted as a completed fix.

## Licensing

The repository's `LICENSE` covers GetPatched-authored recipe metadata and documentation. Expat and its upstream fix are MIT licensed; the original upstream copyright and Debian notices are retained in `LICENSES/`. Preserve those notices in source and binary packages.
