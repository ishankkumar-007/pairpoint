import base64


def encode_base64(input_string):
    # Convert the input string to bytes
    input_bytes = input_string.encode("utf-8")

    # Encode the bytes to Base64
    base64_bytes = base64.b64encode(input_bytes)

    # Convert the Base64 bytes back to a string
    base64_string = base64_bytes.decode("utf-8")

    return base64_string


# Example usage
input_string = "ishank:ishank"
encoded_string = encode_base64(input_string)

print("Encoded Base64:", encoded_string)
