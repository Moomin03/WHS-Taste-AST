# 🧠 Function Analyzer (화이트햇 스쿨 3기 과제)

이 프로젝트는 **화이트햇 스쿨 3기 - 프로그래밍 기초** 수업에서 진행한 과제입니다.  
C언어로 작성되었으며, **C 프로그램의 AST(Abstract Syntax Tree)** 를 분석하여 함수들의 정보를 추출합니다.

## 📌 주요 기능

- C 코드의 AST(JSON 형식)를 파싱
- 함수 정의(`FuncDef`)를 탐색하여:
  - 함수 이름
  - 반환 타입
  - 파라미터 수 및 타입/이름
  - 함수 본문 내 `if` 조건문의 개수
- 결과를 콘솔에 보기 쉽게 출력

## 📁 사용된 기술

- C (GCC 컴파일러)
- [jansson](https://digip.org/jansson/) 라이브러리: JSON 처리용 C 라이브러리

## ⚙️ 설치 및 실행 방법

1. **jansson 라이브러리 설치 (Ubuntu 기준)**

```bash
sudo apt update
sudo apt install libjansson-dev
gcc analyzer.c -o analyzer -ljansson
