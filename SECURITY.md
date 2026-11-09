# Security Policy

## Supported Versions

| Version | Supported |
|---------|-----------|
| Newest release | Yes |
| Older releases | No |

A fix ships as a new release.

## Verifying a Download

The portable ZIP has a Sigstore bundle (`.sigstore.json`) signed with the maintainer's GitHub
account (`cesarboss2022`). GitHub shows each file's SHA-256 beside it. To check the file:

```
uvx sigstore verify identity --cert-identity 97560413+cesarboss2022@users.noreply.github.com --cert-oidc-issuer https://github.com/login/oauth --bundle CourierCrisisRecomp-win-Portable.zip.sigstore.json CourierCrisisRecomp-win-Portable.zip
```

With cosign: `cosign verify-blob --bundle CourierCrisisRecomp-win-Portable.zip.sigstore.json --certificate-identity 97560413+cesarboss2022@users.noreply.github.com --certificate-oidc-issuer https://github.com/login/oauth CourierCrisisRecomp-win-Portable.zip`

The executable is not code-signed, so Windows SmartScreen may warn on first start.

## Reporting a Vulnerability

If you discover a security vulnerability, please report it responsibly through
GitHub's private reporting: open the repository's **Security** tab and choose
**Report a vulnerability**. Include steps to reproduce, the affected release and
the potential impact.

Please do **not** open a public GitHub issue for security vulnerabilities.

## Response Timeline

- Acknowledgement: within 7 days
- Fix or mitigation: depends on severity