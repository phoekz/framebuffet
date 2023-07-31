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
// I/O
//

ConstantBuffer<Constants> g_constants: register(b0);
RWStructuredBuffer<Particle> particles: register(u0);

//
// Entry points
//

[numthreads(128, 1, 1)] void cs_main(Input input) {
    uint i = input.dtid.x;
    Particle p = particles[i];
    p.position.y -= g_constants.speed * g_constants.delta_time;
    if (p.position.y < -1.0f)
        p.position.y = 1.0f;
    particles[i] = p;
}
