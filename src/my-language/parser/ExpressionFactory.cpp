#include "ExpressionFactory.h"

MyLanguage::ExpressionFactory::ExpressionFactory()
{
}

std::shared_ptr<Expression> MyLanguage::ExpressionFactory::createFunctionParameter(
    std::string name,
    std::string type
)
{
    auto parameter = std::shared_ptr<Expression>(new Expression{"function-parameter", -1, -1});
    parameter->subTypes().insert({"name", name});
    parameter->subTypes().insert({"explicit-type", type});
    return parameter;
}

std::shared_ptr<Expression> MyLanguage::ExpressionFactory::createFunction(
    std::string name,
    std::string returnType,
    std::vector<std::shared_ptr<Expression>> parameterExpressions,
    std::vector<std::shared_ptr<Expression>> definitionExpressions
)
{
    // Create function expression.
    auto f = std::shared_ptr<Expression>(new Expression{"function", 0, 0});
    f->subTypes().insert({"name", name});
    f->subTypes().insert({"return-type", returnType});

    // Create parameter list expression.
    auto parameterList = std::shared_ptr<Expression>(new Expression{"function-parameter-list", 0, 0});
    // Populate parameter list.
    std::for_each(parameterExpressions.begin(), parameterExpressions.end(), [&parameterList](auto parameter) {
        Expression::addChild(parameterList, parameter);
    });

    // Create function definition expression.
    auto definition = std::shared_ptr<Expression>(new Expression{"function-definition", 0, 0});
    // Populate definition expression's children.
    std::for_each(definitionExpressions.begin(), definitionExpressions.end(), [&definition](auto statement) {
        Expression::addChild(definition, statement);
    });
    
    // Attach parameter list and definition to the function expression.
    Expression::addChild(f, parameterList);
    Expression::addChild(f, definition);

    // Determine whether the function contains a return statement.
    auto definitionChildren = f->children().at(1)->children();
    int returns = std::count_if(definitionChildren.begin(), definitionChildren.end(), [](auto statement) {
        return statement->type() == "return";
    });
    f->subTypes().insert({"returns-amount", std::to_string(returns)});

    return f;
}