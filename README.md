# Pipes

<img src="demo.gif" alt="Colored pipes with &quot;PIPES&quot;" />

```sh
gcc pipes.c -o pipes -lncursesw && ./pipes
```

Press Ctrl+C to exit

## Workflows

Pin to a release tag (`@v3.0.0`); Renovate bumps it in the callers. In a caller repo, name the files
by role only: `ci.yaml`, `deploy.yaml`, `release.yaml`

Workflows that take a `setup` install that toolchain: `node` reads the caller's `.nvmrc` and runs
`npm ci`, `go` reads `go.mod`, `python` runs `uv sync --locked`, `none` installs nothing. Anything
else the caller needs (a C toolchain, a CLI) goes in `install`, a command or a script. Go tools
belong in a `tools/go.mod` run via `go tool -modfile=tools/go.mod`, so local and CI share one pinned
version; `setup: go` caches every `go.sum`, including that one

### CI (`ci-checks.yaml`)

Runs the given checks as parallel jobs, one per check, each as `<runner> <check>`. Required status
checks show up as `pipes / ci (<check>)`

```yaml
jobs:
    pipes:
        uses: dragunovartem99/pipes/.github/workflows/ci-checks.yaml@v3.0.0
        with:
            setup: node  # optional: node, go, python or none (default)
            runner: npm run  # or make, or any command that takes the check name
            checks: '["format:check", "types:check", "lint:check", "test"]'
            install: sudo apt-get install -y shfmt  # optional: runs after setup, before the checks
            fetch-depth: 0  # optional: full history, e.g. when the build reads `git log` (default 1)
            cache: .cache/og  # optional: restores paths saved by deploy-vps, newline-separated
        secrets:
            build-env: '{"API_URL": "${{ secrets.API_URL }}"}'  # optional, exposed as env vars to every check
```

Typical: `runner: npm run` with `["format:check", "types:check", "lint:check", "test"]` for Node,
`runner: make` with `["fmt-check", "lint", "vet", "test"]` for Go or `["fmt-check", "lint", "test"]`
for Python

### Deploy to GitHub Pages (`deploy-github.yaml`)

Builds and deploys a static site to GitHub Pages

> The caller workflow must grant the required permissions:
> ```txt
> permissions:
>     contents: read
>     pages: write
>     id-token: write
> ```

```yaml
jobs:
    pipes:
        uses: dragunovartem99/pipes/.github/workflows/deploy-github.yaml@v3.0.0
        with:
            setup: node  # optional: node, go, python or none (default)
            build: npm run build
            dist-folder: ./dist
            install: bash scripts/install-toolchain.sh  # optional: runs after setup, before the build
            fetch-depth: 0  # optional, as in CI
        secrets:
            build-env: '{"API_URL": "${{ secrets.API_URL }}"}'  # optional, exposed as env vars to the build step
```

### Deploy to VPS (`deploy-vps.yaml`)

Deploys over SSH to a server that has `VPS_HOST`, `VPS_USER`, `VPS_SSH_KEY` and `VPS_PROJECT_PATH`
as repository secrets. Two strategies:

- **checkout** (default) — rsyncs `upload` into the git checkout at `VPS_PROJECT_PATH`, resets it to
  the pushed commit and runs its `./deploy.sh`. For Docker services
- **release** — rsyncs `upload` into `VPS_PROJECT_PATH-releases/<sha>`, atomically points the
  `VPS_PROJECT_PATH` symlink at it, runs its `deploy.sh` if present and removes the previous release.
  For static sites

```yaml
jobs:
    pipes:
        uses: dragunovartem99/pipes/.github/workflows/deploy-vps.yaml@v3.0.0
        with:
            strategy: release
            setup: node  # optional: node, go, python or none (default)
            build: npm run build  # optional, runs on the runner
            upload: dist/ Caddyfile deploy.sh  # optional
            url: https://example.com  # optional
            install: bash scripts/install-toolchain.sh  # optional: runs after setup, before the build
            fetch-depth: 0  # optional, as in CI
            cache: .cache/og  # optional: paths cached across deploys (and restored by ci-checks), newline-separated
        secrets: inherit
```

`secrets: inherit` can't carry `build-env`; to expose env vars to the build, pass the secrets explicitly:

```yaml
        secrets:
            VPS_HOST: ${{ secrets.VPS_HOST }}
            VPS_USER: ${{ secrets.VPS_USER }}
            VPS_SSH_KEY: ${{ secrets.VPS_SSH_KEY }}
            VPS_PROJECT_PATH: ${{ secrets.VPS_PROJECT_PATH }}
            build-env: '{"API_URL": "${{ secrets.API_URL }}"}'
```

### Release to npm (`release-npm.yaml`)

Opens a release PR while changesets are pending, then publishes to npm and creates the git tag and
GitHub release once that PR is merged

> The caller workflow must grant the required permissions:
> ```txt
> permissions:
>     contents: write
>     pull-requests: write
>     id-token: write
> ```

```yaml
on:
    push:
        branches: ["main"]

jobs:
    pipes:
        uses: dragunovartem99/pipes/.github/workflows/release-npm.yaml@v3.0.0
        with:
        secrets:
            npm-token: ${{ secrets.NPM_TOKEN }}  # optional, omit when the package uses trusted publishing
```

Authenticate one of two ways:

- **Trusted publishing (OIDC)** — omit `npm-token` and set the package's trusted publisher on npmjs.com
  to this repo and workflow. Nothing to store or rotate; the workflow updates npm to a version that
  supports it
- **Token** — add a granular automation token as the `NPM_TOKEN` secret and pass it as above

## Recording

With [asciinema](https://asciinema.org) and [agg](https://github.com/asciinema/agg), in Tomorrow Night colors:

```sh
TERM=xterm asciinema rec --cols 80 --rows 30 -c "timeout --foreground 270.5 ./pipes" raw.cast
# squash the first 4 minutes, drop the exit
jq -c 'if type == "object" then . elif .[0] < 270 and .[1] == "o" then .[0] = ([.[0] - 240, 0] | max) else empty end' \
    raw.cast > demo.cast
agg --font-family "JetBrainsMonoNL Nerd Font Mono" --font-size 32 --line-height 1 --speed 3 --fps-cap 30 --last-frame-duration 0 \
    --theme 1d1f21,c5c8c6,282a2e,cc6666,b5bd68,f0c674,81a2be,b294bb,8abeb7,c5c8c6,969896,cc6666,b5bd68,f0c674,81a2be,b294bb,8abeb7,ffffff \
    demo.cast demo.gif
```
