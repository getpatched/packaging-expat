# GetPatched Expat recipe

This repository is a public maintenance recipe for Debian's Expat source package. It keeps the upstream source separate from Debian's packaging, pins one exact Debian Security source per target, and carries a narrowly scoped backport with its regression probe.

## Candidate targets

| Target | Exact Debian source | Candidate version | Status |
|---|---|---|---|
| Debian 12 Bookworm amd64 | `2.5.0-1+deb12u5` | `2.5.0-1+deb12u5+getpatched1` | Hosted development verification passed; production release pending |
| Debian 13 Trixie amd64 | `2.9.0-1~deb13u1` | `2.9.0-1~deb13u1+getpatched1` | Hosted development verification passed; production release pending |

Both source descriptors and their orig and Debian packaging archives are pinned by SHA-256 in the target files. The hashes match the signed Debian Security `Sources.xz` indexes for immutable snapshot `20261030T000000Z`. The `.dsc` uploader signatures have not been verified separately. Ubuntu, other Debian releases, arm64, and other architectures are not configured here.

## Vulnerability and backport

CVE-2026-117610 is a NULL pointer dereference in `externalEntityInitProcessor2()`. An application that handles external entities can crash when a subparser first receives an empty non-final chunk and later receives document data. Debian's current tracker marks both pinned Security source versions vulnerable. Upstream fixed the issue in commit [`bfc28346fa3efeb8ca56c17437d24a7344e5feab`](https://github.com/libexpat/libexpat/commit/bfc28346fa3efeb8ca56c17437d24a7344e5feab) by handling `XML_TOK_NONE` during initialization. The same small patch applies to both source versions after their Debian quilt series.

The regression fixture calls the installed library through its public C API. It creates an external-entity parser, parses an empty non-final input chunk, resumes with a processing instruction, and verifies the handler ran. A separate `xmlwf -v` check covers a normal command-line consumer.

## Evidence state

The exact `.dsc`, upstream source, and Debian packaging archives for both targets were downloaded and matched against their signed index hashes. Each Debian quilt series applied with zero fuzz, and the GetPatched backport applied to both targets without fuzz. On the local arm64 macOS host, Debian's patched source built with its Autotools flow: the regression crashed with SIGSEGV before the backport and passed after it for both source versions.

Policy-matched hosted Debian package verification passed for Bookworm [run 41543751522](https://github.com/getpatched/platform/actions/runs/41543751522) and Trixie [run 41546270294](https://github.com/getpatched/platform/actions/runs/41546270294). Both reports set `candidateVerified=true`, `verificationChecksPassed=true`, and `releaseEligible=false` because production publication is disabled. In each run, the exact installed baseline reproduced the CVE regression as SIGSEGV, the installed candidate passed it, `xmlwf -v` succeeded on both baseline and candidate, and the candidate-only GetPatched package-aware scan changed from three affected package records (`expat`, `libexpat1`, `libexpat1-dev`) to zero. The runs also built the Debian package and completed isolated signed APT client verification; the temporary repository and signing key were discarded. Full provenance and raw reports are retained in the corresponding platform workflow artifacts.

The pinned Trivy 0.70.0 database did not contain CVE-2026-117610 and reported no finding for this CVE on either image. The before/after 3-to-0 result comes from the isolated GetPatched candidate advisory feed and package-aware dpkg scan, not from Trivy. An earlier Trixie run [41545338753](https://github.com/getpatched/platform/actions/runs/41545338753) was cancelled before producing reports; it is not evidence. No production package, signed production index, or release was published. These results verify only Debian Bookworm/Trixie amd64; Ubuntu and other architectures are not configured or claimed. Platform catalogue publication remains in review in [platform PR #110](https://github.com/getpatched/platform/pull/110), so this recipe is not yet counted as a completed public release.

## Licensing

The repository's `LICENSE` covers GetPatched-authored recipe metadata and documentation. Expat and its upstream fix are MIT licensed; the original upstream copyright and Debian notices are retained in `LICENSES/`. Preserve those notices in source and binary packages.
