import os


def main():
    print("请粘贴以回车分割的多个文件路径，输入空行结束：")
    lines = []
    while True:
        line = input()
        if not line.strip():
            break
        lines.append(line.strip())
    files = [f for f in dict.fromkeys(lines) if f]

    qrc_path = os.path.join(os.path.dirname(__file__), "resources.qrc")
    with open(qrc_path, "w", encoding="utf-8") as f:
        f.write('<RCC>\n\t<qresource prefix="/">\n')
        for file in files:
            f.write(f'\t\t<file>{file}</file>\n')
        f.write('\t</qresource>\n</RCC>\n')

    print(f"已写入 {len(files)} 个文件到 {qrc_path}")


if __name__ == "__main__":
    main()
