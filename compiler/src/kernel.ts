export type Device = {
    name: string
    nodes: {
        name: string
    }[]
}

export type Kernel = {
    get_device(name: string): Device
}