#ifndef HTMLTEMPLATES_HPP
# define HTMLTEMPLATES_HPP
# define DIRLIST "<!DOCTYPE html> \
<html lang=\"en\"> \
<head> \
    <meta charset=\"UTF-8\"> \
    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
    <link rel=\"stylesheet\" href=\"../styles.css\"> \
    <title>Directory PATH </title> \
</head> \
<body> \
    <h1>Directory PATH </h1> \
    <div class=\"formbox\"> \
            INSERT \
    </div> \
    <script> \
        async function deleteFile(filename) { \
            try { \
                const response = await fetch(`${window.location.pathname}/${filename}`, {\
                    method: 'DELETE', \
                    headers: { \
                        'Content-Type': 'application/json', \
                    }, \
                    body: JSON.stringify({ filename }), \
                }); \
                if (response.ok) { \
                    alert(\"File deleted\"); \
                    location.reload(); \
                } else { \
                    alert(\"Error: File not deleted\"); \
                } \
            } catch (error) { \
                console.error('An error occurred:', error); \
            } \
        } \
    </script> \
</body> \
</html>"

#endif
