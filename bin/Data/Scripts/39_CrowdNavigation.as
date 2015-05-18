// CrowdNavigation example.
// This sample demonstrates:
//     - Generating a dynamic navigation mesh into the scene
//     - Performing path queries to the navigation mesh
//     - Adding and removing obstacles at runtime from the dynamic mesh
//     - Adding and removing crowd agents at runtime
//     - Raycasting drawable components
//     - Crowd movement management
//     - Accessing crowd agents with the crowd manager

#include "Scripts/Utilities/Sample.as"

DetourCrowdManager@ crowdManager;

void Start()
{
    // Execute the common startup for samples
    SampleStart();

    // Create the scene content
    CreateScene();

    // Create the UI content
    CreateUI();

    // Setup the viewport for displaying the scene
    SetupViewport();

    // Hook up to the frame update and render post-update events
    SubscribeToEvents();
}

void CreateScene()
{
    scene_ = Scene();

    // Create octree, use default volume (-1000, -1000, -1000) to (1000, 1000, 1000)
    // Also create a DebugRenderer component so that we can draw debug geometry
    scene_.CreateComponent("Octree");
    scene_.CreateComponent("DebugRenderer");

    // Create scene node & StaticModel component for showing a static plane
    Node@ planeNode = scene_.CreateChild("Plane");
    planeNode.scale = Vector3(100.0f, 1.0f, 100.0f);
    StaticModel@ planeObject = planeNode.CreateComponent("StaticModel");
    planeObject.model = cache.GetResource("Model", "Models/Plane.mdl");
    planeObject.material = cache.GetResource("Material", "Materials/StoneTiled.xml");

    // Create a Zone component for ambient lighting & fog control
    Node@ zoneNode = scene_.CreateChild("Zone");
    Zone@ zone = zoneNode.CreateComponent("Zone");
    zone.boundingBox = BoundingBox(-1000.0f, 1000.0f);
    zone.ambientColor = Color(0.15f, 0.15f, 0.15f);
    zone.fogColor = Color(0.5f, 0.5f, 0.7f);
    zone.fogStart = 100.0f;
    zone.fogEnd = 300.0f;

    // Create a directional light to the world. Enable cascaded shadows on it
    Node@ lightNode = scene_.CreateChild("DirectionalLight");
    lightNode.direction = Vector3(0.6f, -1.0f, 0.8f);
    Light@ light = lightNode.CreateComponent("Light");
    light.lightType = LIGHT_DIRECTIONAL;
    light.castShadows = true;
    light.shadowBias = BiasParameters(0.00025f, 0.5f);
    // Set cascade splits at 10, 50 and 200 world units, fade shadows out at 80% of maximum shadow distance
    light.shadowCascade = CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f);

    // Create randomly sized boxes. If boxes are big enough, make them occluders. Occluders will be software rasterized before
    // rendering to a low-resolution depth-only buffer to test the objects in the view frustum for visibility
    for (uint i = 0; i < 20; ++i)
    {
        Node@ boxNode = scene_.CreateChild("Box");
        float size = 1.0f + Random(10.0f);
        boxNode.position = Vector3(Random(80.0f) - 40.0f, size * 0.5f, Random(80.0f) - 40.0f);
        boxNode.SetScale(size);
        StaticModel@ boxObject = boxNode.CreateComponent("StaticModel");
        boxObject.model = cache.GetResource("Model", "Models/Box.mdl");
        boxObject.material = cache.GetResource("Material", "Materials/Stone.xml");
        boxObject.castShadows = true;
        if (size >= 3.0f)
            boxObject.occluder = true;
    }

    // Create a DynamicNavigationMesh component to the scene root
    DynamicNavigationMesh@ navMesh = scene_.CreateComponent("DynamicNavigationMesh");
    // Enable drawing debug geometry for obstacles
    navMesh.drawObstacles = true;
    // Set the agent height large enough to exclude the layers under boxes
    navMesh.agentHeight = 10;
    // Set nav mesh tilesize to something reasonable
    navMesh.tileSize = 64;
    // Set nav mesh cell height to minimum (allows agents to be grounded)
    navMesh.cellHeight = 0.04f;
    // Create a Navigable component to the scene root. This tags all of the geometry in the scene as being part of the
    // navigation mesh. By default this is recursive, but the recursion could be turned off from Navigable
    scene_.CreateComponent("Navigable");
    // Add padding to the navigation mesh in Y-direction so that we can add objects on top of the tallest boxes
    // in the scene and still update the mesh correctly
    navMesh.padding = Vector3(0.0f, 10.0f, 0.0f);
    // Now build the navigation geometry. This will take some time. Note that the navigation mesh will prefer to use
    // physics geometry from the scene nodes, as it often is simpler, but if it can not find any (like in this example)
    // it will use renderable geometry instead
    navMesh.Build();

    // Create a DetourCrowdManager component to the scene root
    crowdManager = scene_.CreateComponent("DetourCrowdManager");

    // Create Jack node as crowd agent
    SpawnJack(Vector3(-5.0f, 0, 20.0f));

    // Create some mushrooms as obstacles. Note that obstacles are added onto an already buit navigation mesh
    for (uint i = 0; i < 100; ++i)
        CreateMushroom(Vector3(Random(90.0f) - 45.0f, 0.0f, Random(90.0f) - 45.0f));

    // Create the camera. Limit far clip distance to match the fog. Note: now we actually create the camera node outside
    // the scene, because we want it to be unaffected by scene load / save
    cameraNode = Node();
    Camera@ camera = cameraNode.CreateComponent("Camera");
    camera.farClip = 300.0f;

    // Set an initial position for the camera scene node above the plane
    cameraNode.position = Vector3(0.0f, 5.0f, 0.0f);
}

void CreateUI()
{
    // Create a Cursor UI element because we want to be able to hide and show it at will. When hidden, the mouse cursor will
    // control the camera, and when visible, it will point the raycast target
    XMLFile@ style = cache.GetResource("XMLFile", "UI/DefaultStyle.xml");
    Cursor@ cursor = Cursor();
    cursor.SetStyleAuto(style);
    ui.cursor = cursor;
    // Set starting position of the cursor at the rendering window center
    cursor.SetPosition(graphics.width / 2, graphics.height / 2);

    // Construct new Text object, set string to display and font to use
    Text@ instructionText = ui.root.CreateChild("Text");
    instructionText.text =
        "Use WASD keys to move, RMB to rotate view\n"
        "LMB to set destination, SHIFT+LMB to spawn a Jack\n"
        "MMB to add obstacles or remove obstacles/agents\n"
        "F5 to save scene, F7 to load\n"
        "Space to toggle debug geometry";
    instructionText.SetFont(cache.GetResource("Font", "Fonts/Anonymous Pro.ttf"), 15);
    // The text has multiple rows. Center them in relation to each other
    instructionText.textAlignment = HA_CENTER;

    // Position the text relative to the screen center
    instructionText.horizontalAlignment = HA_CENTER;
    instructionText.verticalAlignment = VA_CENTER;
    instructionText.SetPosition(0, ui.root.height / 4);
}

void SetupViewport()
{
    // Set up a viewport to the Renderer subsystem so that the 3D scene can be seen
    Viewport@ viewport = Viewport(scene_, cameraNode.GetComponent("Camera"));
    renderer.viewports[0] = viewport;
}

void SubscribeToEvents()
{
    // Subscribe HandleUpdate() function for processing update events
    SubscribeToEvent("Update", "HandleUpdate");

    // Subscribe HandlePostRenderUpdate() function for processing the post-render update event, during which we request
    // debug geometry
    SubscribeToEvent("PostRenderUpdate", "HandlePostRenderUpdate");

    // Subscribe HandleCrowdAgentFailure() function for resolving invalidation issues with agents, during which we
    // use a larger extents for finding a point on the navmesh to fix the agent's position
    SubscribeToEvent("CrowdAgentFailure", "HandleCrowdAgentFailure");
}

Node@ CreateMushroom(const Vector3& pos)
{
    Node@ mushroomNode = scene_.CreateChild("Mushroom");
    mushroomNode.position = pos;
    mushroomNode.rotation = Quaternion(0.0f, Random(360.0f), 0.0f);
    mushroomNode.SetScale(2.0f + Random(0.5f));
    StaticModel@ mushroomObject = mushroomNode.CreateComponent("StaticModel");
    mushroomObject.model = cache.GetResource("Model", "Models/Mushroom.mdl");
    mushroomObject.material = cache.GetResource("Material", "Materials/Mushroom.xml");
    mushroomObject.castShadows = true;

    // Create the navigation Obstacle component and set its height & radius proportional to scale
    Obstacle@ obstacle = mushroomNode.CreateComponent("Obstacle");
    obstacle.radius = mushroomNode.scale.x;
    obstacle.height = mushroomNode.scale.y;

    return mushroomNode;
}

Node@ SpawnJack(const Vector3& pos)
{
    Node@ jackNode = scene_.CreateChild("Jack");
    jackNode.position = pos;
    AnimatedModel@ modelObject = jackNode.CreateComponent("AnimatedModel");
    modelObject.model = cache.GetResource("Model", "Models/Jack.mdl");
    modelObject.material = cache.GetResource("Material", "Materials/Jack.xml");
    modelObject.castShadows = true;

    // Create a CrowdAgent component and set its height (use default radius)
    CrowdAgent@ agent = jackNode.CreateComponent("CrowdAgent");
    agent.height = 2.0f;

    return jackNode;
}

void SetPathPoint()
{
    Vector3 hitPos;
    Drawable@ hitDrawable;
    DynamicNavigationMesh@ navMesh = scene_.GetComponent("DynamicNavigationMesh");

    if (Raycast(250.0f, hitPos, hitDrawable))
    {
        Vector3 pathPos = navMesh.FindNearestPoint(hitPos, Vector3(1.0f, 1.0f, 1.0f));

        if (input.qualifierDown[QUAL_SHIFT])
            // Spawn a jack
            SpawnJack(pathPos);
        else
        {
            // Set target position and init agents' move
            Array<CrowdAgent@>@ agents = crowdManager.GetActiveAgents();
            for (uint i = 0; i < agents.length; ++i)
            {
                CrowdAgent@ agent = agents[i];

                if (i == 0)
                    // The first agent will always move to the exact target
                    agent.SetMoveTarget(pathPos);
                else
                {
                    // Other agents will move to a random point nearby
                    Vector3 targetPos = navMesh.FindNearestPoint(pathPos + Vector3(Random(-4.5f, 4.5f), 0.0f, Random(-4.5, 4.5f)), Vector3(1.0f, 1.0f, 1.0f));
                    agent.SetMoveTarget(targetPos);
                }
            }
        }
    }
}

void AddOrRemoveObject()
{
    // Raycast and check if we hit a mushroom node. If yes, remove it, if no, create a new one
    Vector3 hitPos;
    Drawable@ hitDrawable;

    if (Raycast(250.0f, hitPos, hitDrawable))
    {
        Node@ hitNode = hitDrawable.node;

        // Note that navmesh rebuild happens when the Obstacle component is removed
        if (hitNode.name == "Mushroom")
            hitNode.Remove();
        else if (hitNode.name == "Jack")
            hitNode.Remove();
        else
            CreateMushroom(hitPos);
    }
}

bool Raycast(float maxDistance, Vector3& hitPos, Drawable@& hitDrawable)
{
    hitDrawable = null;

    IntVector2 pos = ui.cursorPosition;
    // Check the cursor is visible and there is no UI element in front of the cursor
    if (!ui.cursor.visible || ui.GetElementAt(pos, true) !is null)
        return false;

    Camera@ camera = cameraNode.GetComponent("Camera");
    Ray cameraRay = camera.GetScreenRay(float(pos.x) / graphics.width, float(pos.y) / graphics.height);
    // Pick only geometry objects, not eg. zones or lights, only get the first (closest) hit
    // Note the convenience accessor to scene's Octree component
    RayQueryResult result = scene_.octree.RaycastSingle(cameraRay, RAY_TRIANGLE, maxDistance, DRAWABLE_GEOMETRY);
    if (result.drawable !is null)
    {
        hitPos = result.position;
        hitDrawable = result.drawable;
        return true;
    }

    return false;
}

void MoveCamera(float timeStep)
{
    // Right mouse button controls mouse cursor visibility: hide when pressed
    ui.cursor.visible = !input.mouseButtonDown[MOUSEB_RIGHT];

    // Do not move if the UI has a focused element (the console)
    if (ui.focusElement !is null)
        return;

    // Movement speed as world units per second
    const float MOVE_SPEED = 20.0f;
    // Mouse sensitivity as degrees per pixel
    const float MOUSE_SENSITIVITY = 0.1f;

    // Use this frame's mouse motion to adjust camera node yaw and pitch. Clamp the pitch between -90 and 90 degrees
    // Only move the camera when the cursor is hidden
    if (!ui.cursor.visible)
    {
        IntVector2 mouseMove = input.mouseMove;
        yaw += MOUSE_SENSITIVITY * mouseMove.x;
        pitch += MOUSE_SENSITIVITY * mouseMove.y;
        pitch = Clamp(pitch, -90.0f, 90.0f);

        // Construct new orientation for the camera scene node from yaw and pitch. Roll is fixed to zero
        cameraNode.rotation = Quaternion(pitch, yaw, 0.0f);
    }

    // Read WASD keys and move the camera scene node to the corresponding direction if they are pressed
    if (input.keyDown['W'])
        cameraNode.Translate(Vector3(0.0f, 0.0f, 1.0f) * MOVE_SPEED * timeStep);
    if (input.keyDown['S'])
        cameraNode.Translate(Vector3(0.0f, 0.0f, -1.0f) * MOVE_SPEED * timeStep);
    if (input.keyDown['A'])
        cameraNode.Translate(Vector3(-1.0f, 0.0f, 0.0f) * MOVE_SPEED * timeStep);
    if (input.keyDown['D'])
        cameraNode.Translate(Vector3(1.0f, 0.0f, 0.0f) * MOVE_SPEED * timeStep);

    // Set destination or spawn a jack with left mouse button
    if (input.mouseButtonPress[MOUSEB_LEFT])
        SetPathPoint();
    // Add new obstacle or remove existing obstacle/agent with middle mouse button
    if (input.mouseButtonPress[MOUSEB_MIDDLE])
        AddOrRemoveObject();

    // Toggle debug geometry with space
    if (input.keyPress[KEY_SPACE])
        drawDebug = !drawDebug;
}

void HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    // Take the frame time step, which is stored as a float
    float timeStep = eventData["TimeStep"].GetFloat();

    // Move the camera, scale movement with time step
    MoveCamera(timeStep);

    // Make the CrowdAgents face the direction of their velocity
    Array<CrowdAgent@>@ agents = crowdManager.GetActiveAgents();
    for (uint i = 0; i < agents.length; ++i)
    {
        CrowdAgent@ agent = agents[i];
        agent.node.worldDirection = agent.actualVelocity;
    }

    // Check for loading/saving the scene from/to the file Data/Scenes/CrowdNavigation.xml relative to the executable directory
    if (input.keyPress[KEY_F5])
    {
        File saveFile(fileSystem.programDir + "Data/Scenes/CrowdNavigation.xml", FILE_WRITE);
        scene_.SaveXML(saveFile);
    }
    if (input.keyPress[KEY_F7])
    {
        File loadFile(fileSystem.programDir + "Data/Scenes/CrowdNavigation.xml", FILE_READ);
        scene_.LoadXML(loadFile);

        // After reload, reacquire crowd manager
        crowdManager = scene_.GetComponent("DetourCrowdManager");

        // Re-enable debug draw for obstacles
        DynamicNavigationMesh@ navMesh = scene_.GetComponent("DynamicNavigationMesh");
        navMesh.drawObstacles = true;
    }
}

void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
{
    // If draw debug mode is enabled, draw navigation debug geometry
    if (drawDebug)
    {
        // Visualize navigation mesh and obstacles
        DynamicNavigationMesh@ navMesh = scene_.GetComponent("DynamicNavigationMesh");
        navMesh.DrawDebugGeometry(true);

        // Visualize agents' path and position to reach
        crowdManager.DrawDebugGeometry(true);
    }
}

void HandleCrowdAgentFailure(StringHash eventType, VariantMap& eventData)
{
    Node@ node = eventData["Node"].GetPtr();
    int state = eventData["CrowdAgentState"].GetInt();

    // If the agent's state is invalid, likely from spawning on the side of a box, find a point in a larger area
    if (state == CrowdAgentState::CROWD_AGENT_INVALID)
    {
        DynamicNavigationMesh@ navMesh = scene_.GetComponent("DynamicNavigationMesh");
        // Get a point on the navmesh using more generous extents
        Vector3 newPos = navMesh.FindNearestPoint(node.position, Vector3(5.0f,5.0f,5.0f));
        // Set the new node position, CrowdAgent component will automatically reset the state of the agent
        node.position = newPos;
    }
}

// Create XML patch instructions for screen joystick layout specific to this sample app
String patchInstructions =
        "<patch>" +
        "    <add sel=\"/element\">" +
        "        <element type=\"Button\">" +
        "            <attribute name=\"Name\" value=\"Button3\" />" +
        "            <attribute name=\"Position\" value=\"-120 -120\" />" +
        "            <attribute name=\"Size\" value=\"96 96\" />" +
        "            <attribute name=\"Horiz Alignment\" value=\"Right\" />" +
        "            <attribute name=\"Vert Alignment\" value=\"Bottom\" />" +
        "            <attribute name=\"Texture\" value=\"Texture2D;Textures/TouchInput.png\" />" +
        "            <attribute name=\"Image Rect\" value=\"96 0 192 96\" />" +
        "            <attribute name=\"Hover Image Offset\" value=\"0 0\" />" +
        "            <attribute name=\"Pressed Image Offset\" value=\"0 0\" />" +
        "            <element type=\"Text\">" +
        "                <attribute name=\"Name\" value=\"Label\" />" +
        "                <attribute name=\"Horiz Alignment\" value=\"Center\" />" +
        "                <attribute name=\"Vert Alignment\" value=\"Center\" />" +
        "                <attribute name=\"Color\" value=\"0 0 0 1\" />" +
        "                <attribute name=\"Text\" value=\"Spawn A jack\" />" +
        "            </element>" +
        "            <element type=\"Text\">" +
        "                <attribute name=\"Name\" value=\"KeyBinding\" />" +
        "                <attribute name=\"Text\" value=\"LSHIFT\" />" +
        "            </element>" +
        "            <element type=\"Text\">" +
        "                <attribute name=\"Name\" value=\"MouseButtonBinding\" />" +
        "                <attribute name=\"Text\" value=\"LEFT\" />" +
        "            </element>" +
        "        </element>" +
        "        <element type=\"Button\">" +
        "            <attribute name=\"Name\" value=\"Button4\" />" +
        "            <attribute name=\"Position\" value=\"-120 -12\" />" +
        "            <attribute name=\"Size\" value=\"96 96\" />" +
        "            <attribute name=\"Horiz Alignment\" value=\"Right\" />" +
        "            <attribute name=\"Vert Alignment\" value=\"Bottom\" />" +
        "            <attribute name=\"Texture\" value=\"Texture2D;Textures/TouchInput.png\" />" +
        "            <attribute name=\"Image Rect\" value=\"96 0 192 96\" />" +
        "            <attribute name=\"Hover Image Offset\" value=\"0 0\" />" +
        "            <attribute name=\"Pressed Image Offset\" value=\"0 0\" />" +
        "            <element type=\"Text\">" +
        "                <attribute name=\"Name\" value=\"Label\" />" +
        "                <attribute name=\"Horiz Alignment\" value=\"Center\" />" +
        "                <attribute name=\"Vert Alignment\" value=\"Center\" />" +
        "                <attribute name=\"Color\" value=\"0 0 0 1\" />" +
        "                <attribute name=\"Text\" value=\"Obstacles\" />" +
        "            </element>" +
        "            <element type=\"Text\">" +
        "                <attribute name=\"Name\" value=\"MouseButtonBinding\" />" +
        "                <attribute name=\"Text\" value=\"MIDDLE\" />" +
        "            </element>" +
        "        </element>" +
        "    </add>" +
        "    <remove sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]/attribute[@name='Is Visible']\" />" +
        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]/element[./attribute[@name='Name' and @value='Label']]/attribute[@name='Text']/@value\">Set</replace>" +
        "    <add sel=\"/element/element[./attribute[@name='Name' and @value='Button0']]\">" +
        "        <element type=\"Text\">" +
        "            <attribute name=\"Name\" value=\"MouseButtonBinding\" />" +
        "            <attribute name=\"Text\" value=\"LEFT\" />" +
        "        </element>" +
        "    </add>" +
        "    <remove sel=\"/element/element[./attribute[@name='Name' and @value='Button1']]/attribute[@name='Is Visible']\" />" +
        "    <replace sel=\"/element/element[./attribute[@name='Name' and @value='Button1']]/element[./attribute[@name='Name' and @value='Label']]/attribute[@name='Text']/@value\">Debug</replace>" +
        "    <add sel=\"/element/element[./attribute[@name='Name' and @value='Button1']]\">" +
        "        <element type=\"Text\">" +
        "            <attribute name=\"Name\" value=\"KeyBinding\" />" +
        "            <attribute name=\"Text\" value=\"SPACE\" />" +
        "        </element>" +
        "    </add>" +
        "</patch>";