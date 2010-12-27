#pragma once

#include "Color24.hpp"
#include "Point.hpp"
#include "GameWorld.hpp"
#include "Logger.hpp"

struct Bounds;
class ConfigScope;

#ifdef MSVC
#pragma warning(push, 0)
#endif

#include <sstream>
#include <string>
#include <vector>

#ifdef MSVC
#pragma warning(pop)
#endif

// game configuration
struct Config
{
	// corresponds to a "scope" from the configuration file (see README)
	class ConfigScope
	{
	public:
		class ScopeCollection
		{
		public:
			// a simple collection of same-typed scopes
			typedef std::vector<ConfigScope> RawScopeCollection;

		private:
			RawScopeCollection collection;
			mutable unsigned long lastIndex;

		public:
			ScopeCollection();

			void Add(const ConfigScope& elem);
			// return true iff there is a scope to be gotten
			bool HasNextScope() const;
			// get the next scope, and advance the "next" pointer
			const ConfigScope& GetNextScope() const;
		};

		// map field name to field value
		typedef std::map<std::string, std::string> FieldMap;
		// map scope name to scope collection
		typedef std::map<std::string, ScopeCollection> ScopeMap;

	private:
		// _bracketCount_ is the count of curly braces
		ConfigScope(std::istream& configInput, long& bracketCount);

		void Init(std::istream& configInput, long& bracketCount);

		// recurse into another configuration scope
		void EnterScope(std::istream&, long& bracketCount);

		FieldMap fields;
		ScopeMap subscopes;

	public:
		ConfigScope(std::istream& configInput);

		bool PeekScope(const std::string& scopeName) const;
		const ConfigScope* GetScope(const std::string& scopeName) const;
	
		// get _fieldName_'s value in the current scope and store in _dest_
		template <typename _T>
		void GetField(const std::string& fieldName, _T& dest) const
		{
			const FieldMap::const_iterator result = fields.find(fieldName);

			if(result == fields.end())
			{
				Logger::Debug(boost::format("Field %1% not found") % fieldName);
				return;
			}

			std::stringstream(result->second) >> dest;
		}
		// special template for getting byte numbers
		template <>
		void GetField<Uint8>(const std::string& fieldName, Uint8& dest) const
		{
			unsigned short dummy;
			GetField(fieldName, dummy);
			dest = dummy;
		}
	};

private:
	Config(ConfigScope);

	static const Config gameConfig;
	static std::stringstream GetDefaultConfig();

public:
	// recurses into the right scope via construction
	struct ConfigLoadable
	{
		ConfigLoadable(const std::string& scopeName, const ConfigScope*& in);
	};

	template <typename _T>
	struct LoadableList : public ConfigLoadable
	{
		typedef std::vector<_T> List;
		typedef typename List::iterator iterator;
		typedef typename List::const_iterator const_iterator;

		List list;

		LoadableList(const std::string& listName, const std::string& elementsName, const ConfigScope* in) :
			ConfigLoadable(listName, in)
		{
			while(in->PeekScope(elementsName))
				list.push_back(_T(in));
		}
			
		iterator begin() { return list.begin(); }
		const_iterator begin() const { return list.begin(); }
		iterator end() { return list.end(); }
		const_iterator end() const { return list.end(); }
	};

	struct ColorData : public ConfigLoadable
	{
		Color24::ColorType r, g, b;

		ColorData(const ConfigScope* in);

		operator Color24() const;
	};

	struct BoundsData : public ConfigLoadable
	{
		Point min, max;

		BoundsData(const ConfigScope* in);

		operator Bounds() const;
	};

	struct SnakeData : public ConfigLoadable
	{
		struct Head : public ConfigLoadable
		{
			ColorData color;

			Head(const ConfigScope* in);
		};

		Head head;
		unsigned long startingLength;
		unsigned short width;
		unsigned short startingSpeed;

		unsigned int speedupPeriod;
		unsigned short speedupAmount;

		double growthRate;
		unsigned long growthCap;

		ColorData color;

		SnakeData(const ConfigScope* in);
	};

	// resource paths
	struct Resources : public ConfigLoadable
	{
		// SFX
		std::string eat;
		std::string spawn;
		std::string die;

		// musical
		std::string theme;

		Resources(const ConfigScope* in);
	};
	
	struct WallData : public ConfigLoadable
	{
		BoundsData bounds;
		ColorData color;

		WallData(const ConfigScope* in);
	};

	struct ScreenData : public ConfigLoadable
	{
		unsigned long w, h;
		ColorData bgColor;

		ScreenData(const ConfigScope* in);
	};

	struct SpawnsData : public ConfigLoadable
	{
		struct SpawnData : public ConfigLoadable
		{
			ColorData color;
			// square size of spawn
			unsigned short size;
			// amount of empty space around spawns
			unsigned short cushion;
			// time before spawn disappears
			unsigned int expiry;
			// spawn rate
			double rate;

			SpawnData(const std::string& spawnScope, const ConfigScope*& in);

			virtual GameWorld::SpawnPtr ConstructSpawn(Point location) const = 0;
		};

		struct FoodData : public SpawnData
		{
			long long points;
			double lengthFactor;
			short speedChange;

			FoodData(const ConfigScope* in);

			GameWorld::SpawnPtr ConstructSpawn(Point location) const;
		};

		struct MineData : public SpawnData
		{
			MineData(const ConfigScope* in);

			GameWorld::SpawnPtr ConstructSpawn(Point location) const;
		};

		typedef std::auto_ptr<SpawnData> SpawnPtr;
		typedef std::vector<SpawnPtr> SpawnList;
		
		// spawn bounds
		BoundsData bounds;
		unsigned int period;
		SpawnList spawnsData;

		SpawnsData(const ConfigScope* in);
	};

	// whether or not music/sound is on
	bool music, sound;

	unsigned short FPS;

	LoadableList<WallData> wallsData;
	ScreenData screen;
	SpawnsData spawns;

	unsigned int pointGainPeriod;
	long long pointGainAmount;

	SnakeData snake;
	Resources resources;

	// get the (only) configuration data
	static const Config& Get();

	static const ConfigScope GetConfigLoader(const std::string& configFileName);
};
