# Third-party notices

This repository vendors (commits) the following third-party assets so that it
remains fully buildable on its own. Each component keeps its own license.

## Relapse-Exploit

The files under `data/src/`, `data/offsets/`, `data/exploit.html` (derived from
`index.html`) and `data/payloads/kexp_2026_05_25.bin` are copied from the
[Relapse-Exploit](https://github.com/ntfargo/Relapse-Exploit) project
(commit `dd8e4e0914c5e9d066ee1f9b056be76cb992c8e0`).

Licensed under the MIT License.

```
Copyright (c) 2026 Nathan Fargo

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

## elfldr

`data/payloads/elfldr-ps5-1360.elf` is a compiled binary of
[ps5-payload-dev/elfldr](https://github.com/ps5-payload-dev/elfldr), licensed
under the GNU General Public License v3.0 (GPL-3.0). Source code is available
at the linked repository.

## esp32_https_server

The [esp32_https_server](https://github.com/fhessel/esp32_https_server) library
(fetched via `lib_deps` in `platformio.ini`) is licensed under the MIT License.

```
Copyright (c) 2017 Frank Hessel
```
