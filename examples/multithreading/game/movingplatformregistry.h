/*! \file movingplatformregistry.h

    Defines a registry for moving platforms
 */
#pragma once
#include <sadvector.h>
#include <sadhash.h>
#include <sadstring.h>

#include <db/dbdatabase.h>

#include "movingplatformstate.h"

class Game;

namespace game
{

/*! Defines a registry for moving platforms
 */
class MovingPlatformRegistry
{
public:
    /*! A new registry
     */
    MovingPlatformRegistry();
    /*! Copies a registry
     *  \param[in] o other registry
     */
    MovingPlatformRegistry(const game::MovingPlatformRegistry& o);
    /*! Copies state from other registry
     *  \param[in] o other registry
     *  \return self-reference
     */
    game::MovingPlatformRegistry& operator=(const game::MovingPlatformRegistry& o);
    /*! Destroys registry
     */
    ~MovingPlatformRegistry();
    /*! Sets database fpr searching for ways
     *  \param[in] game a game for working with
     *  \param[in] db database
     */
    void setDatabase(Game* game, sad::db::Database* db);
    /*! Adds a pair to registry to start them moving. Does nothing, if platform already
     *  exists
     *  \param[in] platform a platform
     *  \param[in] way a way
     *  \return true if added, if something wrong or platform is already exists returns false
     */
    bool add(sad::p2d::Body* platform, sad::Way* way);
    /*! Adds a pair to registry to start them moving. Does nothing, if platform already
     *  exists
     *  \param[in] platform_name a platform name from registry
     *  \param[in] way_name a way name in linked database
     *  \return true if added, if something wrong or platform is already exists returns false
     */
    bool add(const sad::String& platform_name, const sad::String& way_name);
    /*! Removes all instances of platform in registry, stopping it
     *  \param[in,out] platform a platform
     */
    void remove(sad::p2d::Body* platform);
    /*! Removes an instance of platform in registry, stopping it
     *  \param[in] platform_name a name for platform
     */
    void remove(const sad::String& platform_name);
    /*! Clears the registry
     */
    void clear();
    /*! Moves platforms by tick, setting correct speeds and stuff
     *  \param[in] tick a tick size for registry
     */
    void movePlatforms(double tick);
    /*! Adds new platform to registry. If platform is nullptr does nothing
     * \param[in] name name for platform
     * \param[in] platform a platform
     */
    void addPlatform(const sad::String& name, sad::p2d::Body* platform);
private:
    /*! Copy states from other registry
     *  \param[in] o other  registry
     */
    void copyState(const game::MovingPlatformRegistry& o);
    /*! Frees states, clearing the registry
     */
    void destroy();
    /*! A list of states to progress through
     */
    sad::Vector<game::MovingPlatformState> m_states;
    /*! A lists of platforms for registry
     */
    sad::Hash<sad::String, sad::p2d::Body*> m_platforms;
    /*! A linked database for searching for ways
     */
    sad::db::Database* m_db;
    /*! A game link
     */
    Game* m_game;
};

}
