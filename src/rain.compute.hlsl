cbuffer ConstantBuffer: register(b0) {
    float delta_time;
    float speed;
};

struct Particle {
    float3 position;
};
RWStructuredBuffer<Particle> particles: register(u0);

struct Input {
    uint3 gid: SV_GroupID;
    uint3 dtid: SV_DispatchThreadID;
    uint3 gtid: SV_GroupThreadID;
    uint gi: SV_GroupIndex;
};

[numthreads(128, 1, 1)] void cs_main(Input input) {
    uint i = input.dtid.x;
    Particle p = particles[i];
    p.position.y -= speed * delta_time;
    if (p.position.y < -1.0f)
        p.position.y = 1.0f;
    particles[i] = p;
}
