#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>

// 추신...
// 멘토님.. 실행이 되기는 합니다.
// 하지만.. 노가다(?)로 복호화 코드를 작성해서 그런지.. Segmentation 에러가 계속 발생을 합니다...
// 이유를.. 잘 모르겠어서 일단 했던 부분까지는 첨부합니다 ㅠ_ㅠ;;

// Here we go?
void print_expression(json_t *node);
void print_statement(json_t *node);
void print_decl(json_t *node);
void print_param(json_t *param);
void print_compound(json_t *node);
void print_function(json_t *node);

// Expression 출력
void print_expression(json_t *node) {
    if (!node) return;

    const char *type = json_string_value(json_object_get(node, "_nodetype"));

    if (strcmp(type, "Constant") == 0) {
        printf("%s", json_string_value(json_object_get(node, "value")));
    } else if (strcmp(type, "ID") == 0) {
        printf("%s", json_string_value(json_object_get(node, "name")));
    } else if (strcmp(type, "BinaryOp") == 0) {
        printf("(");
        print_expression(json_object_get(node, "left"));
        printf(" %s ", json_string_value(json_object_get(node, "op")));
        print_expression(json_object_get(node, "right"));
        printf(")");
    } else if (strcmp(type, "UnaryOp") == 0) {
        printf("(%s", json_string_value(json_object_get(node, "op")));
        print_expression(json_object_get(node, "expr"));
        printf(")");
    } else if (strcmp(type, "FuncCall") == 0) {
        print_expression(json_object_get(node, "name"));
        printf("(");
        json_t *args = json_object_get(node, "args");
        if (args && json_object_get(args, "exprs")) {
            json_t *exprs = json_object_get(args, "exprs");
            size_t i;
            json_t *arg;
            json_array_foreach(exprs, i, arg) {
                if (i > 0) printf(", ");
                print_expression(arg);
            }
        }
        printf(")");
    }
}
// declaretion 출력
void print_decl(json_t *node) {
    const char *name = json_string_value(json_object_get(node, "name"));
    json_t *type = json_object_get(node, "type");
    const char *typename = json_string_value(json_object_get(type, "names"));

    printf("%s %s;\n", typename ? typename : "int", name ? name : "var");
}
// Statement 출력
void print_statement(json_t *node) {
    if (!node) return;

    const char *type = json_string_value(json_object_get(node, "_nodetype"));

    if (strcmp(type, "Compound") == 0) {
        print_compound(node);
    } else if (strcmp(type, "Return") == 0) {
        printf("return ");
        print_expression(json_object_get(node, "expr"));
        printf(";\n");
    } else if (strcmp(type, "Assignment") == 0) {
        print_expression(json_object_get(node, "lvalue"));
        printf(" %s ", json_string_value(json_object_get(node, "op")));
        print_expression(json_object_get(node, "rvalue"));
        printf(";\n");
    } else if (strcmp(type, "If") == 0) {
        printf("if (");
        print_expression(json_object_get(node, "cond"));
        printf(")\n");
        print_statement(json_object_get(node, "iftrue"));
        if (json_object_get(node, "iffalse")) {
            printf("else\n");
            print_statement(json_object_get(node, "iffalse"));
        }
    } else if (strcmp(type, "Decl") == 0) {
        print_decl(node);
    }
}

// Compound 블록
void print_compound(json_t *node) {
    printf("{\n");
    json_t *block_items = json_object_get(node, "block_items");
    if (block_items) {
        size_t i;
        json_t *stmt;
        json_array_foreach(block_items, i, stmt) {
            print_statement(stmt);
        }
    }
    printf("}\n");
}

// 함수 파라미터
void print_param(json_t *param) {
    const char *type = json_string_value(json_object_get(json_object_get(param, "type"), "names"));
    const char *name = json_string_value(json_object_get(param, "name"));
    printf("%s %s", type ? type : "int", name ? name : "param");
}

// 함수 정의
void print_function(json_t *node) {
    if (!node) return;

    json_t *decl = json_object_get(node, "decl");
    json_t *type = json_object_get(decl, "type");

    const char *func_name = json_string_value(json_object_get(decl, "name"));
    const char *ret_type = json_string_value(json_object_get(json_object_get(type, "type"), "names"));

    printf("%s %s(", ret_type ? ret_type : "int", func_name ? func_name : "noname");

    json_t *params = json_object_get(type, "args");
    if (params) {
        json_t *param_list = json_object_get(params, "params");
        size_t i;
        json_t *param;
        json_array_foreach(param_list, i, param) {
            if (i > 0) printf(", ");
            print_param(param);
        }
    }

    printf(")\n");

    json_t *body = json_object_get(node, "body");
    print_compound(body);
}

// 메인 함수
int main() {
    json_error_t error;
    json_t *root = json_load_file("ast.json", 0, &error);
    if (!root) {
        fprintf(stderr, "JSON 파싱 오류: %s\n", error.text);
        return 1;
    }

    json_t *exts = json_object_get(root, "ext");
    if (!json_is_array(exts)) {
        fprintf(stderr, "ext 배열이 없음\n");
        return 1;
    }

    size_t i;
    json_t *item;
    json_array_foreach(exts, i, item) {
        const char *type = json_string_value(json_object_get(item, "_nodetype"));
        if (strcmp(type, "FuncDef") == 0) {
            print_function(item);
            printf("\n");
        }
    }

    json_decref(root);

    return 0; // Here we go finish..... ha...........;;;
}