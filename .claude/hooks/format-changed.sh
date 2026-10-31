#!/usr/bin/env bash
# Stop / SubagentStop hook: clang-formats the C++ files the checkout has changed (tracked diff
# against HEAD plus untracked files, whoever made the change), once per turn rather than after
# every edit, so exact-match edits never race the formatter. Generated and vendored files are
# skipped. SubagentStop formats only inside a linked worktree (agent worktrees), never
# the main checkout, where the lead may be mid-edit. Never blocks; reports what it formatted.
set -uo pipefail
input="$(cat)"
cwd="$(jq -r '.cwd // empty' <<<"$input")"
event="$(jq -r '.hook_event_name // empty' <<<"$input")"
[[ -n "$cwd" && -d "$cwd" ]] || exit 0
root="$(git -C "$cwd" rev-parse --show-toplevel 2>/dev/null)" || exit 0
if [[ "$event" == SubagentStop &&
    "$(git -C "$cwd" rev-parse --path-format=absolute --git-dir --git-common-dir | sort -u | wc -l)" -eq 1 ]]; then
    exit 0 # a subagent in the main checkout: leave the lead's files alone
fi
clang_format="${CLANG_FORMAT:-/opt/homebrew/opt/llvm/bin/clang-format}"
command -v "$clang_format" >/dev/null 2>&1 || clang_format="$(command -v clang-format)" || exit 0

files=()
while IFS= read -r file; do
    case "$file" in
    third_party/* | build/* | .cache/*) continue ;;
    *.cpp | *.hpp | *.h) [[ -f "$root/$file" ]] && files+=("$root/$file") ;;
    esac
done < <(cd "$root" && { git diff --name-only HEAD 2>/dev/null; git ls-files --others --exclude-standard; } | sort -u)
[[ ${#files[@]} -gt 0 ]] || exit 0

changed=()
for file in "${files[@]}"; do
    before="$(shasum "$file")"
    "$clang_format" -i --style=file "$file" 2>/dev/null || continue
    [[ "$(shasum "$file")" == "$before" ]] || changed+=("${file#"$root"/}")
done
if [[ ${#changed[@]} -gt 0 ]]; then
    printf '{"systemMessage": "clang-format reformatted %d file(s): %s"}\n' "${#changed[@]}" "${changed[*]}"
fi
exit 0
