# Pipes

<img src="demo.gif" alt="Colored pipes growing randomly across the terminal around the PIPES logo" />

```sh
gcc pipes.c -o pipes -lncursesw && ./pipes
```

Press Ctrl+C to exit

## Workflows

Pin to a release tag (`@v1.0.1`); Renovate bumps it in the callers. In a caller repo, name the files
by role only: `ci.yaml`, `deploy.yaml`, `release.yaml`

Node workflows take the Node version from the caller's `.nvmrc`

### CI for Node (`ci-node.yaml`)

Runs the given npm scripts as parallel jobs, one per check

```yaml
jobs:
    pipes:
        uses: dragunovartem99/pipes/.github/workflows/ci-node.yaml@v1.0.1
        with:
            checks: '["format:check", "types:check", "lint:check", "test"]'  # optional, this is the default
```

### CI for Go (`ci-go.yaml`)

Runs the given make targets as parallel jobs, one per check. golangci-lint is installed for `lint`

```yaml
jobs:
    pipes:
        uses: dragunovartem99/pipes/.github/workflows/ci-go.yaml@v1.0.1
        with:
            checks: '["fmt-check", "lint", "vet", "test"]'  # optional, this is the default
            golangci-lint-version: "v2.13"  # optional, this is the default
```

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
        uses: dragunovartem99/pipes/.github/workflows/deploy-github.yaml@v1.0.1
        with:
            build-command: build
            dist-folder: ./dist
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
        uses: dragunovartem99/pipes/.github/workflows/deploy-vps.yaml@v1.0.1
        with:
            strategy: release
            setup: node  # optional: node, go or none (default)
            build: npm run build  # optional, runs on the runner
            upload: dist/ Caddyfile deploy.sh  # optional
            url: https://example.com  # optional
        secrets: inherit
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
        uses: dragunovartem99/pipes/.github/workflows/release-npm.yaml@v1.0.1
        with:
        secrets:
            npm-token: ${{ secrets.NPM_TOKEN }}  # optional, omit when the package uses trusted publishing
```

Authenticate one of two ways:

- **Trusted publishing (OIDC)** — omit `npm-token` and set the package's trusted publisher on npmjs.com
  to this repo and workflow. Nothing to store or rotate; the workflow updates npm to a version that
  supports it
- **Token** — add a granular automation token as the `NPM_TOKEN` secret and pass it as above

## Recording the GIF

With [asciinema](https://asciinema.org) and [agg](https://github.com/asciinema/agg), in Tomorrow Night colors:

```sh
TERM=xterm asciinema rec --cols 80 --rows 30 -c "timeout --foreground 270.5 ./pipes" raw.cast
# start once the screen is filled: squash the first 4 minutes into the opening frame, keep 30s (played back at 3x),
# and drop the exit that would blank the last frame
jq -c 'if type == "object" then . elif .[0] < 270 and .[1] == "o" then .[0] = ([.[0] - 240, 0] | max) else empty end' \
    raw.cast > demo.cast
agg --font-family "JetBrainsMonoNL Nerd Font Mono" --font-size 32 --line-height 1 --speed 3 --fps-cap 30 --last-frame-duration 0 \
    --theme 1d1f21,c5c8c6,282a2e,cc6666,b5bd68,f0c674,81a2be,b294bb,8abeb7,c5c8c6,969896,cc6666,b5bd68,f0c674,81a2be,b294bb,8abeb7,ffffff \
    demo.cast demo.gif
```
