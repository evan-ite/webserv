
read -n "$CONTENT_LENGTH" POST_DATA
IFS="&" read -ra KEY_VALUE_PAIRS <<< "$POST_DATA"
declare -A FORM_FIELDS

for pair in "${KEY_VALUE_PAIRS[@]}"; do
    IFS="=" read -r key value <<< "$pair"
    value=$(echo "$value" | sed 's/%\([0-9A-Fa-f][0-9A-Fa-f]\)/\\x\1/g' | xargs -0 printf "%b")
    FORM_FIELDS["$key"]="$value"
done

FAV="${FORM_FIELDS["favorite"]}"
WORST="${FORM_FIELDS["worst"]}"

cat << EOF
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Simple CGI in bash</title>
    <style>
        body {
            background-color: black;
            color: white;
            font-family: Consolas, Monaco, Lucida Console, Liberation Mono, DejaVu Sans Mono, Bitstream Vera Sans Mono, Courier New, monospace;
            margin: 0;
            padding: 0;
            display: flex;
            flex-direction: column;
            height: 100vh;
        }
        p {
            line-height: 1.2;
            margin: 0;
        }
        .cursor {
            display: inline;
            animation: blink 0.5s infinite alternate;
        }
        @keyframes blink {
            0% {
                opacity: 1;
            }
            100% {
                opacity: 0;
            }
        }
        .button-container {
            margin-top: auto;
            padding: 10px;
            text-align: center;
        }
        .back-button {
            background-color: black;
            color: white;
            border: 1px solid white;
            padding: 5px 10px;
            cursor: pointer;
            text-decoration: none;
            font-family: Consolas, Monaco, Lucida Console, Liberation Mono, DejaVu Sans Mono, Bitstream Vera Sans Mono, Courier New, monospace;
        }
        .back-button:hover {
            background-color: white;
            color: black;
        }
    </style>
</head>
<body>
    <p>bash-3.2$ Your favorite coding language is $FAV</p>
    <p>bash-3.2$ Your most hated coding language is $WORST</p>
    <p>bash-3.2$ <span class="cursor">▓</span></p>
    <div class="button-container">
        <a href="/" class="back-button">Back</a>
    </div>
</body>
</html>
EOF
