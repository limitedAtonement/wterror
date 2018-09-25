#define BOOST_SIGNALS_NO_DEPRECATION_WARNING
#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Sqlite3.h>
#include <Wt/WEnvironment.h>
#include <string>
#include <string>
#include <Wt/Dbo/Dbo.h>

Wt::Dbo::Session session;
struct puppy;
struct trick;
using trick_ptr = Wt::Dbo::ptr<struct trick>;

struct puppy_id
{
    std::string name;
    std::string type;
};

namespace Wt::Dbo
{
    template<>
    struct dbo_traits<puppy> : public dbo_default_traits
    {
        typedef puppy_id IdType;
        static IdType invalidId() {return {};}
        static const char * surrogateIdField() { return nullptr;}
    };
    template <class A>
    void field(A & a, puppy_id & id, std::string const & name, int = -1)
    {
        field(a, id.name, name + "name");
        field(a, id.type, name + "type");
    }
}

struct puppy : public Wt::Dbo::Dbo<puppy>
{
    puppy_id id;
    template<class A> void persist(A&a)
    {
        Wt::Dbo::id(a, id, "");
        Wt::Dbo::hasMany(a, tricks, Wt::Dbo::ManyToOne, "puppyid");
    }
private:
    Wt::Dbo::collection<Wt::Dbo::ptr<trick> > tricks;
};

struct trick : public Wt::Dbo::Dbo<trick>
{
    std::string name;
    Wt::Dbo::ptr<puppy> mypuppy;
    template <class A> void persist(A&a)
    {
        Wt::Dbo::field(a, name, "name");
        Wt::Dbo::belongsTo(a, mypuppy, "puppyid");
    }
};

bool operator <(puppy_id const & lhs, puppy_id const & rhs)
{
    if (lhs.name < rhs.name)
        return true;
    if (lhs.name > rhs.name)
        return true;
    return lhs.type < rhs.type;
}
bool operator==(puppy_id const & lhs, puppy_id const & rhs)
{
    return lhs.name == rhs.name && lhs.type == rhs.type;
}
std::ostream & operator<<(std::ostream & str, puppy_id const & key)
{
    return str << key.name << "," << key.type;
}

int main(int, char**)
{
    auto db {std::make_unique<Wt::Dbo::backend::Sqlite3>("puppies.db")};
    db->setProperty("show-queries", "true");
    session.setConnection(std::move(db));
    session.mapClass<puppy>("puppy");
    session.mapClass<trick>("trick");
    try
    {
        session.createTables();
        std::cerr << "created new database\n";
    }
    catch (Wt::Dbo::Exception & e)
    {
        std::cerr << "New database not created because of ";
        std::cerr << e.what() << ". Assuming db already exists.\n";
    }
    return 0;
}

