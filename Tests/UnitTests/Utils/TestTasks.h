#ifndef UNIT_TESTS_TEST_TASKS_H
#define UNIT_TESTS_TEST_TASKS_H

#include <hspp/Accounts/Player.h>
#include <hspp/Enums/TaskEnums.h>
#include <hspp/Tasks/MetaData.h>
#include <hspp/Tasks/Tasks.h>

#include <functional>

using namespace Hearthstonepp;

class TestTask : public ITask
{
 public:
    TestTask(TaskID id, std::function<MetaData(Player&, Player&)>&& impl);

    TaskID GetTaskID() const override;

 private:
    TaskID m_id;
    std::function<MetaData(Player&, Player&)> m_impl;

    MetaData Impl(Player& player1, Player& player2) override;
};

#endif  // UNITTEST_TEST_TASKS_H