#ifndef TSC_LEVEL_HPP_HPP
#define TSC_LEVEL_HPP_HPP
namespace TSC {

    /**
     * This class represents a level that is being played.
     */
    class cLevel {
    public:
        static cLevel* Load_From_File(boost::filesystem::path levelfile);

#ifdef _DEBUG
        static cLevel* Construct_Debugging_Level();
#endif

        cLevel(boost::filesystem::path levelfile);
        ~cLevel();

        void Update();
        void Draw(sf::RenderWindow& stage) const;

        inline std::string Name() const {return path_to_utf8(m_levelfile.stem());}
        void Sort_Z_Elements();

    private:
        cLevel(); // Private constructor
        void Init();

        boost::filesystem::path m_levelfile;
        int m_engine_version;
        sf::Vector2f m_player_startpos;
        sf::FloatRect m_camera_limits;

        /// Invisible actors that do not need drawing execpt in editor mode.
        std::vector<cActor*> m_nodraw_actors;
        /// Normal actors drawn during regular gameplay.
        std::vector<cSpriteActor*> m_normal_actors;
        /// Level backgrounds.
        std::vector<sf::Sprite*> m_backgrounds;
    };

}
#endif
