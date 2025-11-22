/**
 * Computes the difference between two objects, returning a new object that
 * represents the changes from obj1 to obj2.
 *
 * This function is fully type-safe from the caller's perspective.
 *
 * - Source: https://stackoverflow.com/questions/8431651/getting-a-diff-of-two-json-objects
 * - Posted by Gabriel Gartz
 * - Retrieved 11/5/2025, License - CC-BY-SA 4.0
 * - Refactored for full TypeScript type-safety.
 *
 * @param obj1 The original object.
 * @param obj2 The new object.
 * @returns A partial object containing only the changed keys, or an empty object
 *          if there are no differences.
 */
export function diff<T extends object>(obj1: T, obj2: T): Partial<T> {
    if (Object.is(obj1, obj2)) {
        return {};
    }
    const result: any = {};
    const allKeys = new Set([...Object.keys(obj1), ...Object.keys(obj2)]);

    for (const key of allKeys) {
        const keyOfT = key as keyof T;
        const val1 = obj1[keyOfT];
        const val2 = obj2[keyOfT];

        if (typeof val1 === 'object' && val1 !== null && typeof val2 === 'object' && val2 !== null) {
            const value = diff(val1 as any, val2 as any);
            if (Object.keys(value).length > 0) {
                result[keyOfT] = val2;
            }
        }
        else if (!Object.is(val1, val2)) {
            result[keyOfT] = val2;
        }
    }
    if (Object.keys(result).length > 0) {
        return result as Partial<T>;
    }
    return {};
}
