//
// Types
//

struct Constants {
    float delta_time;
    float speed;
};

struct Particle {
    float3 position;
};

struct Input {
    uint3 gid: SV_GroupID;
    uint3 dtid: SV_DispatchThreadID;
    uint3 gtid: SV_GroupThreadID;
    uint gi: SV_GroupIndex;
};

//
// Bindings
//

struct Bindings {
    uint constants;
    uint particles;
};
ConstantBuffer<Bindings> g_bindings: register(b0);

//
// Entry points
//

[numthreads(128, 1, 1)] void cs_main(Input input) {
    ConstantBuffer<Constants> constants = ResourceDescriptorHeap[g_bindings.constants];
    RWStructuredBuffer<Particle> particles = ResourceDescriptorHeap[g_bindings.particles];

    uint i = input.dtid.x;
    Particle p = particles[i];
    p.position.y -= constants.speed * constants.delta_time;
    if (p.position.y < -1.0f)
        p.position.y = 1.0f;
    particles[i] = p;
}
