import os


def collect_files(root_dir):
    files = []
    for dirpath, _, filenames in os.walk(root_dir):
        for filename in filenames:
            full_path = os.path.join(dirpath, filename)
            # Path relative to the project root directory
            rel_path = os.path.relpath(full_path, os.path.dirname(__file__))
            files.append(rel_path.replace("\\", "/"))
    return files


def main():
    dist_dir = os.path.join(os.path.dirname(__file__), "web/vue-proj/dist")
    if not os.path.exists(dist_dir):
        print(f"Directory does not exist: {dist_dir}")
        return

    files = collect_files(dist_dir)
    if not files:
        print("No files detected.")
        return

    qrc_path = os.path.join(os.path.dirname(__file__), "resources.qrc")
    with open(qrc_path, "w", encoding="utf-8") as f:
        f.write('<RCC>\n\t<qresource prefix="/">\n')
        for file in files:
            f.write(f'\t\t<file>{file}</file>\n')
        f.write('\t</qresource>\n</RCC>\n')

    print(f"Wrote {len(files)} files to {qrc_path}")


if __name__ == "__main__":
    main()
