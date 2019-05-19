#include "../test_helpers.hxx"

#include <pqxx/except>

using namespace pqxx;

namespace
{
void test_exceptions()
{
  const std::string
    broken_query = "SELECT HORRIBLE ERROR",
    err = "Error message";

  try
  {
    throw sql_error{err, broken_query};
  }
  catch (const std::exception &e)
  {
    PQXX_CHECK_EQUAL(
	e.what(),
	err,
	"Exception contains wrong message.");
    const sql_error *downcast{dynamic_cast<const sql_error *>(&e)};
    PQXX_CHECK_NOT_EQUAL(
	downcast,
	nullptr,
	"exception-to-sql_error downcast is broken.");
    PQXX_CHECK_EQUAL(
	downcast->query(),
	broken_query,
	"Getting query from pqxx exception is broken.");
  }

  connection conn;
  work tx{conn};
  try
  {
    tx.exec("INVALID QUERY HERE");
  }
  catch (const syntax_error &e)
  {
    // SQL syntax error has sqlstate error 42601.
    PQXX_CHECK_EQUAL(
	e.sqlstate(), "42601", "Unexpected sqlstate on syntax error.");
  }
}


PQXX_REGISTER_TEST(test_exceptions);
} // namespace
