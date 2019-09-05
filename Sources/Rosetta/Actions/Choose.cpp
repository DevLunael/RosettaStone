// This code is based on Sabberstone project.
// Copyright (c) 2017-2019 SabberStone Team, darkfriend77 & rnilva
// RosettaStone is hearthstone simulator using C++ with reinforcement learning.
// Copyright (c) 2019 Chris Ohk, Youngjoong Kim, SeungHyun Jeon

#include <Rosetta/Actions/Choose.hpp>
#include <Rosetta/Actions/Generic.hpp>

#include <algorithm>

namespace RosettaStone::Generic
{
void ChoiceMulligan(Player& player, const std::vector<std::size_t>& choices)
{
    // Block it if player tries to mulligan in a non-mulligan choice
    if (player.choice.value().choiceType != ChoiceType::MULLIGAN)
    {
        return;
    }

    // Block it if player tries to mulligan a card that doesn't exist
    Choice& choice = player.choice.value();
    for (const auto chooseID : choices)
    {
        if (std::find(choice.choices.begin(), choice.choices.end(), chooseID) ==
            choice.choices.end())
        {
            return;
        }
    }

    // Process mulligan by choice action
    switch (choice.choiceAction)
    {
        case ChoiceAction::HAND:
        {
            // Process mulligan state
            player.mulliganState = Mulligan::DEALING;

            auto& hand = player.GetHandZone();
            auto& deck = player.GetDeckZone();

            // Collect cards to redraw
            std::vector<Entity*> mulliganList;
            for (const auto entity : hand.GetAll())
            {
                const bool isExist = std::find(choices.begin(), choices.end(),
                                               static_cast<size_t>(entity->id)) == choices.end();
                if (isExist && entity->card->id != "GAME_005")
                {
                    mulliganList.push_back(entity);
                }
            }

            // Process redraw
            for (const auto& entity : mulliganList)
            {
                Entity& newCard = deck.Remove(*deck.GetTopCard());
                AddCardToHand(player, &newCard);
                hand.Swap(*entity, newCard);

                hand.Remove(*entity);
                deck.Add(*entity);
                deck.Shuffle();
            }

            // It's done! - Reset choice
            player.choice = std::nullopt;

            break;
        }
        default:
            throw std::invalid_argument(
                "ChoiceMulligan() - Invalid choice action!");
    }
}

bool ChoicePick(Player& player, std::size_t choice)
{
    // Block it if player tries to pick in a non-general choice
    if (player.choice.value().choiceType != ChoiceType::GENERAL)
    {
        return false;
    }

    // Block it if player tries to pick a card that doesn't exist
    if (std::find(player.choice.value().choices.begin(),
                  player.choice.value().choices.end(),
                  choice) == player.choice.value().choices.end())
    {
        return false;
    }

    // Get picked card using entity ID
    Entity* entity = player.GetSetasideZone().GetEntity(choice);
    // Block it if player tries to pick a card that doesn't exist
    if (entity == nullptr)
    {
        return false;
    }

    // Process pick by choice action
    switch (player.choice.value().choiceAction)
    {
        case ChoiceAction::HAND:
        {
            player.GetSetasideZone().Remove(*entity);
            AddCardToHand(player, entity);
            break;
        }
        default:
            throw std::invalid_argument(
                "ChoiceMulligan() - Invalid choice action!");
    }

    return true;
}

void CreateChoice(Player& player, ChoiceType type, ChoiceAction action,
                  const std::vector<std::size_t>& choices)
{
    // Block it if choice is exist
    if (player.choice != std::nullopt)
    {
        return;
    }

    // Create a choice for player
    Choice choice;
    choice.choiceType = type;
    choice.choiceAction = action;
    choice.choices = choices;
    player.choice = choice;
}
}  // namespace RosettaStone::Generic
