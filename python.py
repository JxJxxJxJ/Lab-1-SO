import re


def hex_to_ascii(hex_string):
    # Elimino '\\x' de cada byte y convierto HEX a BYTES
    hex_string = hex_string.replace("\\x", "")
    bytes_obj = bytes.fromhex(hex_string)
    # Convierto BYTES a ASCII
    return bytes_obj.decode("ascii")


def replace_hex_in_file(file_path):
    with open(file_path, "r", encoding="utf-8") as file:
        content = file.read()

    # Regexp para secuencias HEX (e.g., '\x55\x4e\x4b')
    hex_pattern = re.compile(r"(\\x[0-9A-Fa-f]{2})+")

    def replace_hex_match(match):
        hex_string = match.group(0)
        return hex_to_ascii(hex_string)

    # Reemplazo cada secuencia HEX con su equivalente ASCII
    # Nota: No captura caracteres de nueva línea \x0a (\n) ni caracteres nulos \x00 ("")
    # Pero lo puedo hacer a mano en vim...
    new_content = hex_pattern.sub(replace_hex_match, content)

    with open(file_path, "w", encoding="utf-8") as file:
        file.write(new_content)

    print(f"Se ha convertido los strings HEX a ASCII en {file_path}")


# Filepath
file_path = "./obfuscated.c"
replace_hex_in_file(file_path)
