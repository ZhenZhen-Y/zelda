
function startGame()
    loadSpriteSheet("objects.csv");
    loadSpriteSheet("food.csv");
    loadSpriteSheet("hero.csv");
    loadSpriteSheet("npc.csv");
    loadSpriteSheet("log.csv");

    loadDialogues("story.xml");
    loadCharacterDefines("npc.xml");
    loadItemDefines("items.xml");
    loadMap("map.tmx");
    loadEnvironmentLightSheet("envlight.csv");

    --createNpc("hero", "", 50, 50, 1);
    createCharacter("tom", "tom", 47, 45, 1);
    createCharacter("log", "log1", 49, 45, 2);
    addItem("log1", "apple", 1);
    addItem("log1", "coin", 2);
    addItem("log1", "watermelon", 5);

    createCharacter("log", "log2", 50, 45, 2);
    addItem("log2", "coin", 2);

    createCharacter("tom", "tom1", 47, 47, 1);

    createItem("pineapple", 52, 50);
    createItem("pineapple", 53, 50);
    createItem("pineapple", 52, 51);
    createItem("pineapple", 53, 51);
    createItem("pineapple", 54, 51);
    createItem("watermelon", 55, 51);
    createItem("apple", 45, 60);
    createItem("apple", 55, 49);
    createItem("coin", 55, 55);
    createItem("fire", 51, 51);

    elapse(5);
end