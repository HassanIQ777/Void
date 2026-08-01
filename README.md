# void

`void` is like `rm`, except it overwrites a file's contents with random data before deleting it, so the bytes aren't just sitting on disk waiting to be undeleted by the first recovery tool that comes along.

## Usage

```
void <FILE1> <FILE2> ...
void -i <VALUE> <FILE>
void -h
void -v
```

| Flag | Description |
|---|---|
| `-h` | Print help |
| `-v` | Print version |
| `-i <VALUE>` | Number of overwrite passes (default: 3) |
| *(none)* | Void one or more files |

### Examples

```sh
# void a single file, 3 passes (default)
void secret.txt

# void with 7 passes, for the paranoid
void -i 7 secret.txt

# void several files in one go
void draft1.txt draft2.txt notes.md
```

## How it works

For each file, `void`:

1. Overwrites the entire file with random bytes, `N` times (`-i`, default 3)
2. Truncates it to 0 bytes
3. Unlinks it from the filesystem

This is the same general approach as `shred`: destroy the content before you remove the pointer to it, so there's nothing left for a naive undelete tool to find.

## What this does *not* protect against

To be upfront about it: overwrite-then-delete is a strong defense against casual recovery (`rm` alone leaves file contents fully intact on disk until something else happens to reuse that space — tools like `photorec` or `testdisk` can often pull it right back). It is **not** a guarantee against a determined attacker with the right access, and the reason comes down to how modern storage actually works:

- **SSDs** don't overwrite data in place. A "write" to an existing file's location is typically redirected by the drive's firmware to a fresh, pre-erased block, while the original block is marked stale and reclaimed later via garbage collection — on the drive's own schedule, not yours. The old data can sit untouched for a while even after `void` has "overwritten" it.
- **Wear leveling** compounds this — SSDs deliberately spread writes across cells to avoid burning out one physical spot, which works directly against the idea of repeatedly overwriting one exact location.
- **Copy-on-write / journaling filesystems** (btrfs, ZFS, APFS, and to a degree ext4's journal) can write updates to new blocks for crash-safety reasons, independent of the SSD question entirely.
- **Snapshots and backups** taken before you run `void` are untouched by anything `void` does to the live file.

If you need a real guarantee against forensic recovery on an SSD, that generally means the drive's own ATA Secure Erase / NVMe Sanitize command, or full-disk encryption with key destruction — not file-level overwriting. `void` is a solid step up from plain `rm`, not a substitute for those.

## Building

```sh
git clone --recursive https://github.com/HassanIQ777/Void.git
cd Void
make
```

> If you already cloned without `--recursive`, run `git submodule update --init` to pull in `libutils`.

## Exit status

`void` returns `0` if every file was successfully voided, and `1` if any file was missing or failed to be removed — safe to check in scripts.

## License

MIT — see [LICENSE](LICENSE).