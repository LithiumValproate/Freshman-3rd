import os


def collect_files(root_dir):
    files = []
    for dirpath, _, filenames in os.walk(root_dir):
        for filename in filenames:
            full_path = os.path.join(dirpath, filename)
            # 路径相对于项目根目录
            rel_path = os.path.relpath(full_path, os.path.dirname(__file__))
            files.append(rel_path.replace("\\", "/"))
    return files


def main():
    dist_dir = os.path.join(os.path.dirname(__file__), "web/vue-proj/dist")
    if not os.path.exists(dist_dir):
        print(f"目录不存在: {dist_dir}")
        return

    files = collect_files(dist_dir)
    if not files:
        print("未检测到任何文件。")
        return

    qrc_path = os.path.join(os.path.dirname(__file__), "resources.qrc")
    with open(qrc_path, "w", encoding="utf-8") as f:
        f.write('<RCC>\n\t<qresource prefix="/">\n')
        for file in files:
            f.write(f'\t\t<file>{file}</file>\n')
        f.write('\t</qresource>\n</RCC>\n')

    print(f"已写入 {len(files)} 个文件到 {qrc_path}")


if __name__ == "__main__":
    main()
