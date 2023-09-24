#include <functional>
#include <iostream>
#include <memory>
#include <utility>

class Child;

struct RootData
{
    int n;
    std::shared_ptr<Child> child;
};

class Root
{
public:
    Root()
            : m_rootData{ new RootData{}}
    {

    }

    void setN(int n) const
    {
        m_rootData->n = n;
    }

    void setChild(std::shared_ptr<Child> child) const
    {
        m_rootData->child = std::move(child);
    }

    [[nodiscard]] int getN() const
    {
        return m_rootData->n;
    }

    [[nodiscard]] std::shared_ptr<Child> getChild() const
    {
        return m_rootData->child;
    }

private:
    std::shared_ptr<RootData> m_rootData;
};

struct ChildData
{
    double r;
};

class Child
{
public:
    Child()
            : m_childData{ new ChildData{}}
    {

    }

    void setR(double r) const
    {
        m_childData->r = r;
    }

    [[nodiscard]] double getR() const
    {
        return m_childData->r;
    }

private:
    std::shared_ptr<ChildData> m_childData;
};

template<typename T, typename U>
class Builder
{
public:
    explicit Builder(U& parent)
            : m_parent{ parent }
    {

    }

    [[nodiscard]] virtual T build() const = 0;

    U& back()
    {
        return m_parent;
    }

    virtual ~Builder() = default;

private:
    std::reference_wrapper<U> m_parent;
};

template<typename T>
class Builder<T, void>
{
public:
    [[nodiscard]] virtual T build() const = 0;

    void back() = delete;

    virtual ~Builder() = default;
};

class ChildBuilder;

class RootBuilder : public Builder<Root, void>
{
public:
    RootBuilder& setN(int n)
    {
        m_root.setN(n);
        return *this;
    }

    ChildBuilder& getChildBuilder()
    {
        if (m_childBuilder == nullptr)
        {
            m_childBuilder = std::make_shared<ChildBuilder>(*this);
        }
        return *m_childBuilder;
    }

    [[nodiscard]] Root build() const override;

private:
    Root m_root;
    std::shared_ptr<ChildBuilder> m_childBuilder;
};

class ChildBuilder : public Builder<Child, RootBuilder>
{
public:
    explicit ChildBuilder(RootBuilder& parent)
            : Builder{ parent }
    {

    }

    ChildBuilder& setR(double r)
    {
        m_child.setR(r);
        return *this;
    }

    [[nodiscard]] Child build() const override
    {
        return m_child;
    }

private:
    Child m_child;
};

Root RootBuilder::build() const
{
    if (m_childBuilder != nullptr)
    {
        m_root.setChild(std::make_shared<Child>(m_childBuilder->build()));
    }
    return m_root;
}

std::ostream& operator<<(std::ostream& os, const Child& child)
{
    os << "Child={" << child.getR() << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Root& root)
{
    os << "Root={" << root.getN() << ',' << *root.getChild() << "}";
    return os;
}

int main()
{
    auto data{ RootBuilder{}
                       .setN(1)
                       .getChildBuilder()
                       .setR(3.14)
                       .back()
                       .build()
    };
    std::cout << data << '\n';
    return 0;
}
