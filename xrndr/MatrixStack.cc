#include <xrndr/MatrixStack.hh>

namespace xrndr
{

MatrixStackSet::MatrixStackSet(const MatrixStackSetSettings & settings)
    : _projectionStack("projection")
    , _viewStack("view")
    , _modelStack("model")
{
    _projectionStack.reserve(settings.projectionMatrixStackSize);
    _viewStack.reserve(settings.viewMatrixStackSize);
    _modelStack.reserve(settings.modelMatrixStackSize);

    _projectionStack.push(mat4());
    _viewStack.push(mat4());
    _modelStack.push(mat4());
}

MatrixStackSet::TransformationScope MatrixStackSet::transformProjection(const mat4 & matrix, bool additive)
{
    return TransformationScope(*this, _projectionStack, matrix, additive);
}

MatrixStackSet::TransformationScope MatrixStackSet::transformView(const mat4 & matrix, bool additive)
{
    return TransformationScope(*this, _viewStack, matrix, additive);
}

MatrixStackSet::TransformationScope MatrixStackSet::transformModel(const mat4 & matrix, bool additive)
{
    return TransformationScope(*this, _modelStack, matrix, additive);
}

const MatrixGroup & MatrixStackSet::getMatrixGroup()
{
    return _group;
}


//


MatrixStack::MatrixStack(const char * name)
    : _name(name)
{
}

bool MatrixStack::empty() const
{
    return _imp.empty();
}

const MatrixStack::Value & MatrixStack::top() const
{
    return _imp.top();
}

MatrixStack::Value & MatrixStack::top()
{
    return _imp.top();
}

void MatrixStack::push(const Value & object)
{
    _imp.push(object);
}

void MatrixStack::pop()
{
    _imp.pop();
}

void MatrixStack::clear()
{
    _imp.clear();
}

void MatrixStack::reserve(size_t count)
{
    _imp.reserve(count);
}

void MatrixStack::shrintToFit()
{
    _imp.shrinkToFit();
}


//


MatrixStackSet::TransformationScope::TransformationScope(MatrixStackSet & matrixStackSet, MatrixStack & matrixStack, const mat4 & matrix, bool additive)
    : _matrixStackSet(matrixStackSet)
    , _matrixStack(matrixStack)
{
    if (!additive || !matrixStack.empty())
    {
        if (additive)
            _matrixStack.push(matrix * _matrixStack.top());
        else
            _matrixStack.push(matrix);

        _updateGroup();
    }
    else
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "cannot apply additive transformation: stack is empty\n");
}

MatrixStackSet::TransformationScope::~TransformationScope()
{
    if (!_matrixStack.empty())
    {
        _matrixStack.pop();
        _updateGroup();
    }
    else
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "cannot pop from stack: stack is empty\n");

}

void MatrixStackSet::TransformationScope::_updateGroup()
{
    auto & group = _matrixStackSet._group;
    auto & modelStack = _matrixStackSet._modelStack;
    auto & viewStack = _matrixStackSet._viewStack;
    auto & projectionStack = _matrixStackSet._projectionStack;

    if (projectionStack.empty())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "cannot update matrix group: projection matrix stack is empty\n");
        return;
    }

    if (viewStack.empty())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "cannot update matrix group: view matrix stack is empty\n");
        return;
    }

    if (modelStack.empty())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "cannot update matrix group: model matrix stack is empty\n");
        return;
    }

    group.modelMatrix = modelStack.top();
    group.modelViewMatrix = viewStack.top() * group.modelMatrix;
    group.modelViewProjectionMatrix = projectionStack.top() * group.modelViewMatrix;
    group.normalMatrix = mat3(transpose(inverse(group.modelMatrix)));
}

}