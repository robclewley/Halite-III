#ifndef COMMANDTRANSACTION_HPP
#define COMMANDTRANSACTION_HPP

#include <functional>

#include "Location.hpp"
#include "GameEvent.hpp"

namespace hlt {

class Map;

struct Player;

/** Base transaction class independent of commands. */
class BaseTransaction {
protected:
    std::function<void(GameEvent)> callback;    /**< The game event callback. */
    Map &map;                                   /**< The game map. */
public:
    /**
     * Construct BaseTransaction from Map.
     * @param map The Map to update.
     * @param player The Player.
     */
    explicit BaseTransaction(Map &map) : map(map) {}

    /**
     * Set a callback for GameEvents generated during the transaction commit.
     * @param callback The callback to set.
     */
    void set_callback(std::function<void(GameEvent)> &&callback) { this->callback = callback; }

    /**
     * Check if the transaction may be committed without actually committing.
     * @return False if the transaction may not be committed.
     */
    virtual bool check() const = 0;

    /** If the transaction may be committed, commit the transaction. */
    virtual void commit() = 0;
};

/**
 * Transactions that execute a series of player commands atomically.
 * @tparam Command The type of the command.
 */
template<class Command>
class Transaction : public BaseTransaction {
protected:
    using Commands = std::vector<std::reference_wrapper<const Command>>; /**< The type of the player command list. */
    std::unordered_map<Player, Commands> commands;                       /**< The stored commands per player. */
public:
    using BaseTransaction::BaseTransaction;

    /**
     * Add a command to the transaction.
     * @param player The player executing the command.
     * @param command The command to be executed.
     */
    void add_command(Player &player, const Command &command) {
        commands[player].emplace_back(command);
    }
};

class DumpCommand;

class ConstructCommand;

class MoveCommand;

class SpawnCommand;

/** Transaction for DumpCommand. */
class DumpTransaction : public Transaction<DumpCommand> {
public:
    using Transaction::Transaction;

    /**
     * Check if the transaction may be committed without actually committing.
     * @return False if the transaction may not be committed.
     */
    bool check() const override;

    /** If the transaction may be committed, commit the transaction. */
    void commit() override;
};

/** Transaction for ConstructCommand. */
class ConstructTransaction : public Transaction<ConstructCommand> {
public:
    using Transaction::Transaction;

    /**
     * Check if the transaction may be committed without actually committing.
     * @return False if the transaction may not be committed.
     */
    bool check() const override;

    /** If the transaction may be committed, commit the transaction. */
    void commit() override;
};

/** Transaction for MoveCommand. */
class MoveTransaction : public Transaction<MoveCommand> {
public:
    using Transaction::Transaction;

    /**
     * Check if the transaction may be committed without actually committing.
     * @return False if the transaction may not be committed.
     */
    bool check() const override;

    /** If the transaction may be committed, commit the transaction. */
    void commit() override;
};

/** Transaction for SpawnCommand. */
class SpawnTransaction : public Transaction<SpawnCommand> {
public:
    using Transaction::Transaction;

    /**
     * Check if the transaction may be committed without actually committing.
     * @return False if the transaction may not be committed.
     */
    bool check() const override;

    /** If the transaction may be committed, commit the transaction. */
    void commit() override;
};

/** Transaction for all commands. */
class CommandTransaction : public BaseTransaction {
public:
    DumpTransaction dump_transaction;           /**< The DumpCommand transaction. */
    ConstructTransaction construct_transaction; /**< The ConstructCommand transaction. */
    MoveTransaction move_transaction;           /**< The MoveCommand transaction. */
    SpawnTransaction spawn_transaction;         /**< The SpawnCommand transaction. */

    /** All of the transactions. */
    std::vector<std::reference_wrapper<BaseTransaction>> all_transactions{dump_transaction, construct_transaction,
                                                                          move_transaction, spawn_transaction};

    /**
     * Add a DumpCommand to the transaction.
     * @param player The player executing the command.
     * @param command The command.
     */
    void add_command(Player &player, const DumpCommand &command);

    /**
     * Add a ConstructCommand to the transaction.
     * @param player The player executing the command.
     * @param command The command.
     */
    void add_command(Player &player, const ConstructCommand &command);

    /**
     * Add a MoveCommand to the transaction.
     * @param player The player executing the command.
     * @param command The command.
     */
    void add_command(Player &player, const MoveCommand &command);

    /**
     * Add a SpawnCommand to the transaction.
     * @param player The player executing the command.
     * @param command The command.
     */
    void add_command(Player &player, const SpawnCommand &command);

    /**
     * Check if the transaction may be committed without actually committing.
     * @return False if the transaction may not be committed.
     */
    bool check() const override;

    /** If the transaction may be committed, commit the transaction. */
    void commit() override;

    /**
     * Construct CommandTransaction from Map.
     * @param map The Map.
     */
    explicit CommandTransaction(Map &map) :
            BaseTransaction(map),
            dump_transaction(map),
            construct_transaction(map),
            move_transaction(map),
            spawn_transaction(map) {}
};

}

#endif // COMMANDTRANSACTION_HPP
