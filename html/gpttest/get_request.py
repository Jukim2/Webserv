#!/usr/bin/env python3

# 라이브러리 import
import cgi

# GET 요청 파라미터 추출
form = cgi.FieldStorage()
name = form.getvalue('name')
age = form.getvalue('age')

# HTML 출력
print("""
<!DOCTYPE html>
<html lang="ko">
<head>
    <meta charset="UTF-8">
    <title>GET 요청 처리</title>
</head>
<body>
<h1>GET 요청 정보</h1>
<p>이름: {}</p>
<p>나이: {}</p>
</body>
</html>
""".format(name, age))
