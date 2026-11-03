# GetPatched Expat recipe

This repository is a public maintenance recipe for Debian's Expat source package. It keeps the upstream source separate from Debian's packaging, pins one exact Debian Security source per target, and carries a narrowly scoped backport with its regression probe.

## Candidate targets

| Target | Exact Debian source | Candidate version | Status |
|---|---|---|---|
| Debian 12 Bookworm amd64 | `2.5.0-1+deb12u5` | `2.5.0-1+deb12u5+getpatched1` | CVE-2026-117610 candidate; hosted checks passed |
| Debian 13 Trixie amd64 | `2.9.0-1~deb13u1` | `2.9.0-1~deb13u1+getpatched2` | CVE-2026-117610 and CVE-2026-113351 hosted checks passed; no production release |

Both source descriptors and their orig and Debian packaging archives are pinned by SHA-256 in the target files. The hashes match the signed Debian Security `Sources.xz` indexes for immutable snapshot `20261030T000000Z`. The `.dsc` uploader signatures have not been verified separately. Ubuntu, other Debian releases, arm64, and other architectures are not configured here.

## Vulnerability and backport

CVE-2026-117610 is a NULL pointer dereference in `externalEntityInitProcessor2()`. An application that handles external entities can crash when a subparser first receives an empty non-final chunk and later receives document data. Debian's current tracker marks both pinned Security source versions vulnerable. Upstream fixed the issue in commit [`bfc28346fa3efeb8ca56c17437d24a7344e5feab`](https://github.com/libexpat/libexpat/commit/bfc28346fa3efeb8ca56c17437d24a7344e5feab) by handling `XML_TOK_NONE` during initialization. The same small patch applies to both source versions after their Debian quilt series.

The regression fixture calls the installed library through its public C API. It creates an external-entity parser, parses an empty non-final input chunk, resumes with a processing instruction, and verifies the handler ran. A separate `xmlwf -v` check covers a normal command-line consumer.

The Trixie candidate also backports CVE-2026-113351, an allocator-accounting denial of service after `XML_ParserReset()`. Its public-API probe parses a valid 70 MiB attribute value, resets the same parser, and parses a small document. The exact vulnerable Trixie binary must fail that second parse with `XML_ERROR_NO_MEMORY`; the patched binary must accept it. A fresh parser must accept the same small document as a control. Debian Bookworm's Expat 2.5.0 predates the affected allocation tracker, so this fix is intentionally Trixie-only. The backport is upstream Expat PR [#1431](https://github.com/libexpat/libexpat/pull/1431), applied without fuzz to the pinned Expat 2.9.0 source after Debian's quilt series.

## Evidence state

The exact `.dsc`, upstream source, and Debian packaging archives for both targets were matched against their signed index hashes. Each Debian quilt series applied with zero fuzz. The CVE-2026-113351 backport also applies without fuzz to the pinned Trixie source. A local source build reproduces `out of memory` after parser reset on the unpatched source and passes after the backport; this is source-build evidence only.

Policy-matched hosted Debian package verification passed for Bookworm [run 41543751522](https://github.com/getpatched/platform/actions/runs/41543751522) and the original Trixie candidate [run 41546270294](https://github.com/getpatched/platform/actions/runs/41546270294). Those runs covered CVE-2026-117610, including the exact installed baseline crash, fixed candidate, `xmlwf -v`, package-aware scans, and isolated signed APT client verification. Full provenance and raw reports are retained in the platform workflow artifacts.

The hosted Debian Trixie `+getpatched2` build and verification [run 41666465396](https://github.com/getpatched/platform/actions/runs/41666465396) passed both installed CVE regressions, the `xmlwf -v` consumer check, paired scans, and a temporary signed APT client install. The CVE-2026-113351 probe reproduced `XML_ERROR_NO_MEMORY` after reset on baseline and passed on the candidate; a fresh parser passed as a control. The report records `candidateVerified=true`, `verificationChecksPassed=true`, and `releaseEligible=false`. The GetPatched package-aware scan moved from six findings across `expat`, `libexpat1`, and `libexpat1-dev` to zero. Trivy 0.70.0 still reports CVE-2026-113351 for the custom `+getpatched2` binaries because its database does not recognize this backport as a fixed version; this candidate is not represented as Trivy-clean. The isolated repository and temporary signing key were discarded after the client check. Publication remains disabled: no production package or signed production index was released. These results cover Debian Trixie amd64 only for CVE-2026-113351; Ubuntu, other Debian releases, and other architectures are not configured or claimed.

## Licensing

The repository's `LICENSE` covers GetPatched-authored recipe metadata and documentation. Expat and its upstream fix are MIT licensed; the original upstream copyright and Debian notices are retained in `LICENSES/`. Preserve those notices in source and binary packages.
