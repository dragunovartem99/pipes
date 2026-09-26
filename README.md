# Pipes

<img src="demo.gif" alt="Colored pipes growing randomly across the terminal around the PIPES logo" />

```sh
gcc pipes.c -o pipes -lncursesw && ./pipes
```

Press Ctrl+C to exit

## Workflows

Pin to the `v1` tag; it moves with every backwards-compatible release

### CI (`ci.yaml`)

Runs the given npm scripts as parallel jobs, one per check

```yaml
jobs:
    pipes:
        uses: dragunovartem99/pipes/.github/workflows/ci.yaml@v1
        with:
            checks: '["format:check", "types:check", "lint:check", "test"]'  # optional, this is the default
            node-version: "24"  # optional, defaults to the runner's preinstalled Node
```

### Deploy (`deploy.yaml`)

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
        uses: dragunovartem99/pipes/.github/workflows/deploy.yaml@v1
        with:
            build-command: build
            dist-folder: ./dist
        secrets:
            build-env: '{"API_URL": "${{ secrets.API_URL }}"}'  # optional, exposed as env vars to the build step
```

### Go CI (`go-ci.yaml`)

Checks gofmt, lints with golangci-lint, vets, runs the tests with the race detector and builds

```yaml
jobs:
    pipes:
        uses: dragunovartem99/pipes/.github/workflows/go-ci.yaml@v1
        with:
            golangci-lint-version: "v2.13"  # optional, this is the default
```

### Deploy to VPS (`deploy-vps.yaml`)

Deploys over SSH to a server that has `VPS_HOST`, `VPS_USER`, `VPS_SSH_KEY` and `VPS_PROJECT_PATH`
as repository secrets. Two strategies:

- **checkout** (default) — rsyncs `upload` into the git checkout at `VPS_PROJECT_PATH`, resets it to
  the pushed commit and runs its `./deploy.sh`. For Docker services
- **release** — rsyncs `upload` into `VPS_PROJECT_PATH-releases/<sha>`, atomically points the
  `VPS_PROJECT_PATH` symlink at it, runs its `deploy.sh` if present and keeps two older releases for
  rollback. For static sites

```yaml
jobs:
    pipes:
        uses: dragunovartem99/pipes/.github/workflows/deploy-vps.yaml@v1
        with:
            strategy: release
            setup: node  # optional: node, go or none (default)
            build: npm run build  # optional, runs on the runner
            upload: dist/ Caddyfile deploy.sh  # optional
            url: https://example.com  # optional
        secrets: inherit
```

### Release (`release.yaml`)

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
        uses: dragunovartem99/pipes/.github/workflows/release.yaml@v1
        with:
            node-version: "24"  # optional, defaults to the runner's preinstalled Node
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
