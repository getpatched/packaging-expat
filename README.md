# GetPatched Expat recipe

This repository is a public maintenance recipe for Debian's Expat source package. It keeps the upstream source separate from Debian's packaging, pins one exact Debian Security source per target, and carries a narrowly scoped backport with its regression probe.

## Candidate targets

| Target | Exact Debian source | Candidate version | Status |
|---|---|---|---|
| Debian 12 Bookworm amd64 | `2.5.0-1+deb12u5` | `2.5.0-1+deb12u5+getpatched1` | CVE-2026-117610 candidate; hosted checks passed |
| Debian 13 Trixie amd64 | `2.9.0-1~deb13u1` | `2.9.0-1~deb13u1+getpatched2` | CVE-2026-117610 verified candidate; CVE-2026-113351 verification pending |

Both source descriptors and their orig and Debian packaging archives are pinned by SHA-256 in the target files. The hashes match the signed Debian Security `Sources.xz` indexes for immutable snapshot `20261030T000000Z`. The `.dsc` uploader signatures have not been verified separately. Ubuntu, other Debian releases, arm64, and other architectures are not configured here.

## Vulnerability and backport

CVE-2026-117610 is a NULL pointer dereference in `externalEntityInitProcessor2()`. An application that handles external entities can crash when a subparser first receives an empty non-final chunk and later receives document data. Debian's current tracker marks both pinned Security source versions vulnerable. Upstream fixed the issue in commit [`bfc28346fa3efeb8ca56c17437d24a7344e5feab`](https://github.com/libexpat/libexpat/commit/bfc28346fa3efeb8ca56c17437d24a7344e5feab) by handling `XML_TOK_NONE` during initialization. The same small patch applies to both source versions after their Debian quilt series.

The regression fixture calls the installed library through its public C API. It creates an external-entity parser, parses an empty non-final input chunk, resumes with a processing instruction, and verifies the handler ran. A separate `xmlwf -v` check covers a normal command-line consumer.

The Trixie candidate also backports CVE-2026-113351, an allocator-accounting denial of service after `XML_ParserReset()`. Its public-API probe parses a valid 70 MiB attribute value, resets the same parser, and parses a small document. The exact vulnerable Trixie binary must fail that second parse with `XML_ERROR_NO_MEMORY`; the patched binary must accept it. A fresh parser must accept the same small document as a control. Debian Bookworm's Expat 2.5.0 predates the affected allocation tracker, so this fix is intentionally Trixie-only. The backport is upstream Expat PR [#1431](https://github.com/libexpat/libexpat/pull/1431), applied without fuzz to the pinned Expat 2.9.0 source after Debian's quilt series.

## Evidence state

The exact `.dsc`, upstream source, and Debian packaging archives for both targets were matched against their signed index hashes. Each Debian quilt series applied with zero fuzz. The CVE-2026-113351 backport also applies without fuzz to the pinned Trixie source. A local source build reproduces `out of memory` after parser reset on the unpatched source and passes after the backport; this is source-build evidence only.

Policy-matched hosted Debian package verification passed for Bookworm [run 41543751522](https://github.com/getpatched/platform/actions/runs/41543751522) and Trixie [run 41546270294](https://github.com/getpatched/platform/actions/runs/41546270294). Both reports set `candidateVerified=true`, `verificationChecksPassed=true`, and `releaseEligible=false` because production publication is disabled. In each run, the exact installed baseline reproduced the CVE regression as SIGSEGV, the installed candidate passed it, `xmlwf -v` succeeded on both baseline and candidate, and the candidate-only GetPatched package-aware scan changed from three affected package records (`expat`, `libexpat1`, `libexpat1-dev`) to zero. The runs also built the Debian package and completed isolated signed APT client verification; the temporary repository and signing key were discarded. Full provenance and raw reports are retained in the corresponding platform workflow artifacts.

The pinned Trivy 0.70.0 database did not contain CVE-2026-117610 and reported no finding for this CVE on either image. The before/after 3-to-0 result comes from the isolated GetPatched candidate advisory feed and package-aware dpkg scan, not from Trivy. The new CVE-2026-113351 `+getpatched2` candidate's installed-package regressions, paired scans, consumer checks, and signed APT client test are still pending hosted verification. No production package, signed production index, or release was published. These results verify only Debian Bookworm/Trixie amd64; Ubuntu and other architectures are not configured or claimed. Platform catalogue publication remains in review in [platform PR #110](https://github.com/getpatched/platform/pull/110), so this recipe is not yet counted as a completed public release.

## Licensing

The repository's `LICENSE` covers GetPatched-authored recipe metadata and documentation. Expat and its upstream fix are MIT licensed; the original upstream copyright and Debian notices are retained in `LICENSES/`. Preserve those notices in source and binary packages.
