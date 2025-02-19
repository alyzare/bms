#ifndef CONSTANTS_H
#define CONSTANTS_H
const uint8_t checkmarkBitmap[] = {
    0b00000000,
    0b10000000,
    0b01000000,
    0b00100000,
    0b00010001,
    0b00001010,
    0b00000100,
    0b00000000};

const uint8_t hitmarkBitmap[] = {
    0b10000001,
    0b01000010,
    0b00100100,
    0b00011000,
    0b00011000,
    0b00100100,
    0b01000010,
    0b10000001};

const uint8_t broadcastBitmap[] = {
    0b10000100, 0b00,
    0b00000010, 0b01,
    0b01001001, 0b10,
    0b10000101, 0b10,
    0b10110101, 0b10,
    0b10110101, 0b10,
    0b10000101, 0b10,
    0b01001001, 0b10,
    0b00000010, 0b01,
    0b10000100, 0b00};

const uint8_t directBitmap[] = {
    0b00000000, 0b00,
    0b10000111, 0b11,
    0b10000100, 0b00,
    0b10110100, 0b00,
    0b11111111, 0b11,
    0b11111111, 0b11,
    0b10110100, 0b00,
    0b10000100, 0b00,
    0b10000111, 0b11,
    0b00000000, 0b00};

const char htmlText[] = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Network Configuration</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 20px;
        }

        form {
            max-width: 400px;
            margin: auto;
            padding: 20px;
            border: 1px solid #ccc;
            border-radius: 10px;
            box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
        }

        label {
            display: block;
            margin-bottom: 8px;
        }

        input,
        select {
            width: 100%;
            padding: 8px;
            margin-bottom: 10px;
            border: 1px solid #ccc;
            border-radius: 5px;
        }

        button {
            width: 48%;
            padding: 10px;
            margin-top: 10px;
            border: none;
            border-radius: 5px;
            cursor: pointer;
        }

        button[type="submit"] {
            background-color: #4CAF50;
            color: white;
        }

        button[type="button"] {
            background-color: #f44336;
            color: white;
        }
    </style>
</head>

<body>
    <form id="networkForm">
        <label for="ssid">SSID:</label>
        <input type="text" id="ssid" name="ssid" required>

        <label for="password">Password:</label>
        <input type="password" id="password" name="password">

        <button type="submit">Submit</button>
        <button type="button" onclick="deleteConfig()">Delete</button>
    </form>

    <script>
        document.getElementById('networkForm').addEventListener('submit', function (event) {
            event.preventDefault();
            
            var formData = new FormData(this);
            fetch('/config', {
                method: 'POST',
                body: formData
            })
            .then(response => response.json())
            .then(data => {
                console.log('Success:', data);
            })
            .catch((error) => {
                console.error('Error:', error);
            });

            alert('Form submitted');
        });

        function deleteConfig() {
            fetch('/config', {
                method: 'DELETE'
            });
            alert('Configuration deleted');
        }
    </script>
</body>

</html>
)rawliteral";
#endif
